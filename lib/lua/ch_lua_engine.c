/*
 * =====================================================================================
 *
 *       Filename:  ch_lua_engine.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/12/2018 11:41:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_lua_engine.h"
#include "ch_log.h"
#include "ch_util.h"

typedef struct {
    
    const void *data;
    size_t len;
}lua_chunk_part_t;

typedef struct {
    ch_lua_engine_t *lua_engine;
    int index;
}lua_load_srpt_t;


static int lua_loadfrom_file(ch_lua_engine_t *lua_engine);

static const char* 
load_reader(lua_State* L ch_unused, void* user_data, size_t* size) 
{
	lua_load_srpt_t *load_srpt = (lua_load_srpt_t *)user_data;
	lua_chunk_part_t *part;
	ch_lua_engine_t *lua_engine =load_srpt->lua_engine;

	/* Do we have more chunks to return? */
	if (load_srpt->index ==lua_engine->chunk_parts->nelts) 
	{
		return NULL;
	}

	/* Get one chunk. */
	part = ((lua_chunk_part_t **)lua_engine->chunk_parts->elts)[load_srpt->index];
   
    *size = part->len;

	load_srpt->index++;

	return (const char *)part->data;
}

static int dump_writer_mem(lua_State * L ch_unused, const void* data, size_t len,void* user_data) 
{
	ch_lua_engine_t *lua_engine = (ch_lua_engine_t *)user_data;
	
	lua_chunk_part_t *part;
	
	void *part_data;

	part_data = ch_palloc(lua_engine->mp, len);
    
    memcpy(part_data, data, len);
    
	part = (lua_chunk_part_t*)ch_palloc(lua_engine->mp,sizeof(lua_chunk_part_t));

	part->data = part_data;
	part->len = len;

	*(const lua_chunk_part_t **)ch_array_push(lua_engine->chunk_parts) =part;

	return 0;
}

#ifndef LUA_PATH_SEP
#define LUA_PATH_SEP ";"
#endif

#define AUX_MARK "\1"

static void lua_set_path(lua_State *L, int tab_idx,
    const char *fieldname, const char *path, const char *default_path)
{
    const char          *tmp_path;

    /* XXX here we use some hack to simplify string manipulation */
    tmp_path = luaL_gsub(L, path, LUA_PATH_SEP LUA_PATH_SEP,
            LUA_PATH_SEP AUX_MARK LUA_PATH_SEP);


    tmp_path = luaL_gsub(L, tmp_path, AUX_MARK, default_path);

    printf("lua setting lua package.%s to \"%s\"\n", fieldname, tmp_path);

    lua_remove(L, -2);

    /* fix negative index as there's new data on stack */
    tab_idx = (tab_idx < 0) ? (tab_idx - 1) : tab_idx;
    lua_setfield(L, tab_idx, fieldname);
}


static int init_lua_vm(ch_lua_engine_t *engine){
    
    lua_State *L;
    const char *old_path;
    size_t old_path_len;

    const char *old_cpath;
    size_t old_cpath_len;
    
    L = luaL_newstate();

    if(L == NULL){
        return -1;
    }
    
    luaL_openlibs(L);

    /*set lua package path*/
    lua_getglobal(L,"package");

    if(engine->lua_path){
    
        lua_getfield(L, -1, "path"); /* get original package.path */
        old_path = lua_tolstring(L, -1, &old_path_len);

        lua_set_path(L, -2, "path", engine->lua_path, old_path);

        lua_pop(L, 1);
    }
    
    if(engine->lua_cpath){
    
            lua_getfield(L, -1, "cpath"); /* get original package.cpath */
            old_cpath = lua_tolstring(L, -1, &old_cpath_len);

            lua_set_path(L, -2, "cpath", engine->lua_cpath, old_cpath);


            lua_pop(L, 1);
    }
    
    engine->lua_state = L;
    
    return 0;
}


static int lua_script_compile(ch_lua_engine_t *lua_engine){

    int rc;

    /*if is_cache == 1,then lua script will be compile and write into memory,
     * this will be faster when exe it*/

    if(lua_engine->is_cache == 1) {

        /*create a array to cache lua scipt*/
        lua_engine->chunk_parts = ch_array_make(lua_engine->mp,32,sizeof(lua_chunk_part_t*));

        if(lua_engine->chunk_parts == NULL){
            return -1;
        }

        rc = lua_loadfrom_file(lua_engine);

        if(rc!=0){
			ch_log(CH_LOG_ERR,"Cannot load lua script from file:%s",lua_engine->lua_fname);
            return -1;
        }

        rc = lua_dump(lua_engine->lua_state,dump_writer_mem,(void*)lua_engine);

        /*failed*/
        if(rc!=0){
            char *errmsg;
            size_t len;

            /*get error msg from lua stack top*/
            ch_lua_stack_get_string(lua_engine->lua_state,&errmsg,&len,-1);
            if(errmsg){
                ch_log(CH_LOG_ERR,"dump lua stript===%s failed from file  [%s]",lua_engine->lua_fname,errmsg);

            }
            return -1;
        }
    }else{
        lua_engine->chunk_parts = NULL;
    }

    return 0;
}

static int lua_loadfrom_cache(ch_lua_engine_t *lua_engine){

    int rc;

    lua_load_srpt_t load_srpt;
    load_srpt.lua_engine = lua_engine;
    load_srpt.index = 0;

    rc = lua_load(lua_engine->lua_state,load_reader,(void*)&load_srpt,lua_engine->lua_fname);
    
    /*load failed*/
    if(rc!=LUA_OK){

        char *errmsg;
        size_t len;

        /*get error msg from lua stack top*/
        ch_lua_stack_get_string(lua_engine->lua_state,&errmsg,&len,-1);
        if(errmsg){
            ch_log(CH_LOG_ERR,"Load lua stript===%s failed from memory cache [%s]",lua_engine->lua_fname,errmsg);

        }

        return -1; 
    }

    return 0;
}

static int lua_loadfrom_file(ch_lua_engine_t *lua_engine){
    
    int rc;
    
    rc = luaL_loadfile(lua_engine->lua_state,lua_engine->lua_fname);
    
    /*load failed*/
    if(rc!=LUA_OK){

        char *errmsg;
        size_t len;

        /*get error msg from lua stack top*/
        ch_lua_stack_get_string(lua_engine->lua_state,&errmsg,&len,-1);
        if(errmsg){
			ch_log(CH_LOG_ERR,"Load lua stript===%s failed from file  [%s]",lua_engine->lua_fname,errmsg);

        }

        return -1; 
    }

    return 0;
}

static int lua_script_load(ch_lua_engine_t *lua_engine) {

    int rc;
    size_t len;
    char *errmsg;

    /*load from cache*/
    if(lua_engine->chunk_parts){

        rc = lua_loadfrom_cache(lua_engine);
    }else{
        /*load from file*/
        rc = lua_loadfrom_file(lua_engine);
    }

    if(rc){

        return -1;
    }

    if(LUA_OK!=lua_pcall(lua_engine->lua_state,0,0,0)){

        /*get error msg from lua stack top*/
        ch_lua_stack_get_string(lua_engine->lua_state,&errmsg,&len,-1);
        if(errmsg){
            ch_log(CH_LOG_ERR,"run lua stript===%s failed  [%s]",lua_engine->lua_fname,errmsg);
        }

        return -1;

    }

    return 0;

} 

static int lua_script_init(ch_lua_engine_t *lua_engine) {
    

	if(lua_engine->lua_init_fun==NULL)
		return 0;

    /*push init entry function into stack top*/
    lua_getglobal(lua_engine->lua_state,lua_engine->lua_init_fun);
    
    if(ch_lua_call(lua_engine->lua_state,0,0,0)!=0){
        return -1;
    }

    return 0;

}

ch_lua_engine_t * ch_lua_engine_create(ch_pool_t *mp,const char *lua_path,
        const char *lua_cpath,
        const char *lua_init_fun,
        const char *lua_run_fun,
        const char *lua_timeout_fun,
        const char *lua_fin_fun,
        int is_cache,
        const char *lua_fname,
        const char *data_key,
        void *data,
        uint64_t timeout){


    ch_lua_engine_t *lua_engine = (ch_lua_engine_t*)ch_palloc(mp,sizeof(*lua_engine));

    lua_engine->mp = mp;
    lua_engine->lua_path = lua_path;
    lua_engine->lua_cpath = lua_cpath;
    lua_engine->lua_init_fun = lua_init_fun;
    lua_engine->lua_run_fun = lua_run_fun;
    lua_engine->lua_timeout_fun = lua_timeout_fun;
    lua_engine->lua_fin_fun = lua_fin_fun;

    lua_engine->is_cache = is_cache;
    lua_engine->lua_fname = lua_fname;
    lua_engine->chunk_parts = NULL;

    if(init_lua_vm(lua_engine)){

        ch_log(CH_LOG_ERR,"Init lua engine state failed!");
        return NULL;
    }

    if(lua_script_compile(lua_engine)){

        ch_log(CH_LOG_ERR,"Compile lua script error!");
        return NULL;
    }

    if(lua_script_load(lua_engine)){

        ch_log(CH_LOG_ERR,"load lua script error!");
        return NULL;

    }
    
    if(data_key){

        ch_lua_engine_udata_set(lua_engine,data_key,data);
    }

    if(lua_script_init(lua_engine)){

        ch_log(CH_LOG_ERR,"Run lua script:%s,init fun:%s failed!",lua_engine->lua_fname,lua_engine->lua_init_fun);

        return NULL;
    }

    
    lua_engine->timeout = timeout;
    lua_engine->last_time = ch_get_current_timems()/1000;

    return lua_engine;
}

int ch_lua_engine_run(ch_lua_engine_t *lua_engine,const char *idata_key,void *idata,
        const char *odata_key,void *odata){

    /*nothing to do*/
    if(lua_engine->lua_run_fun == NULL)
        return 0;

    if(idata){

        ch_lua_engine_udata_set(lua_engine,idata_key,idata);
    }
    
    if(odata){

        ch_lua_engine_udata_set(lua_engine,odata_key,odata);
    }



    /*push run entry function into stack top*/
    lua_getglobal(lua_engine->lua_state,lua_engine->lua_run_fun);
    
    if(ch_lua_call(lua_engine->lua_state,0,0,0)!=0){
        
        ch_log(CH_LOG_ERR,"Run lua script:%s,run fun:%s failed!",lua_engine->lua_fname,lua_engine->lua_run_fun);
        return -1;
    }

    return 0;
}

int ch_lua_engine_timeout(ch_lua_engine_t *lua_engine,const char *idata_key,void *idata,
        const char *odata_key,void *odata){

    uint64_t cur_time;

    /*nothing to do*/
    if(lua_engine->lua_timeout_fun == NULL)
        return 0;

    cur_time = ch_get_current_timems()/1000;

    if(cur_time-lua_engine->last_time<lua_engine->timeout){

        return 0;
    }

    lua_engine->last_time = cur_time;

    if(idata){

        ch_lua_engine_udata_set(lua_engine,idata_key,idata);
    }
    
    if(odata){

        ch_lua_engine_udata_set(lua_engine,odata_key,odata);
    }

    /*push run entry function into stack top*/
    lua_getglobal(lua_engine->lua_state,lua_engine->lua_timeout_fun);
    
    if(ch_lua_call(lua_engine->lua_state,0,0,0)!=0){
        
        ch_log(CH_LOG_ERR,"Run lua script:%s,timeout fun:%s failed!",lua_engine->lua_fname,lua_engine->lua_timeout_fun);
        return -1;
    }

    return 0;
}


void ch_lua_engine_fin(ch_lua_engine_t *lua_engine){

    /*nothing to do*/
    if(lua_engine->lua_fin_fun == NULL)
        return;

    /*push run entry function into stack top*/
    lua_getglobal(lua_engine->lua_state,lua_engine->lua_fin_fun);
    
    ch_lua_call(lua_engine->lua_state,0,0,0);
}

