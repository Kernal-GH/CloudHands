/*
 *
 *      Filename: ch_lua_filter_engine.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-11 12:21:39
 * Last Modified: 2017-11-11 16:05:21
 */

#include "ch_lua_filter_engine.h"
#include "ch_log.h"
#include "ch_lua_script.h"
#include "ch_config.h"
#include "ch_lua_packet_api.h"

static int lua_script_init(lua_State *state,void *user_data){
    
	ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)user_data;

	if(engine->lua_init_fun==NULL)
		return 0;

    /*push init entry function into stack top*/
    lua_getglobal(state,engine->lua_init_fun);
    
#if 0
    /*push args*/
    lua_pushstring(engine->state,engine->file_url);
#endif
    if(ch_lua_call(state,0,0,0)!=0){
        return -1;
    }

    return 0;

}

static int lua_script_run(lua_State *state,ch_packet_t *packet,void * user_data){

	ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)user_data;

	ch_lua_packet_api_set(state,packet);

    /*nothing to do*/
    if(engine->lua_run_fun == NULL)
        return 0;

    /*push match entry function into stack top*/
    lua_getglobal(state,engine->lua_run_fun);
    

    return ch_lua_match_call(state,0);
}

static int lua_script_run_fin(lua_State *state ch_unused,void * user_data ch_unused){

    return 0;
}

static ch_lua_script_ops_t ops={
    .lua_run_init = lua_script_init, 
    .lua_run = lua_script_run ,
    .lua_run_fin = lua_script_run_fin 
};

static const char * cmd_lua_path(cmd_parms * cmd ch_unused, void *_dcfg, const char *p1){

    ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)_dcfg;
    engine->lua_path = p1;
    
    return NULL;
}

static const char * cmd_lua_cpath(cmd_parms * cmd ch_unused, void *_dcfg, const char *p1){

    ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)_dcfg;
    engine->lua_cpath = p1;
    
    return NULL;
}

static const char * cmd_lua_run_fun(cmd_parms * cmd ch_unused, void *_dcfg, const char *p1){

    ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)_dcfg;
    engine->lua_run_fun = p1;
    
    return NULL;
}

static const char * cmd_lua_init_fun(cmd_parms * cmd ch_unused, void *_dcfg, const char *p1){

    ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)_dcfg;
    engine->lua_init_fun = p1;
    
    return NULL;
}

static const char * cmd_lua_fin_fun(cmd_parms * cmd ch_unused, void *_dcfg, const char *p1){

    ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)_dcfg;
    engine->lua_fin_fun = p1;
    
    return NULL;
}

static uint64_t gid = 10000;

static const char * cmd_lua_script(cmd_parms * cmd ch_unused, void *_dcfg,
                                   const char *p1, const char *p2,const char *p3){
    ch_lua_script_t *lua_script;

    int lua_cache = 0;
    ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)_dcfg;
    
    if(strcasecmp(p3,"on")==0){
        lua_cache = 1;
    }

    lua_script = ch_lua_script_create(engine->mp,&ops,p1,gid++,p2,lua_cache);

    if(!lua_script){
        return "create lua script failed!";
    }

    /*add to lua engine*/
    list_add_tail(&lua_script->node,&engine->scripts);
    
    return NULL;
}

static const command_rec lua_directives[] ={
    
    CH_INIT_TAKE1(
            "CHLuaPath",
            cmd_lua_path,
            NULL,
            0,
            "set lua script path"
            ),

    CH_INIT_TAKE1(
            "CHLuaCPath",
            cmd_lua_cpath,
            NULL,
            0,
            "set lua script c lib path"
            ),

    CH_INIT_TAKE1(
            "CHLuaRunFun",
            cmd_lua_run_fun,
            NULL,
            0,
            "set lua script run function name"
            ),

    CH_INIT_TAKE1(
            "CHLuaInitFun",
            cmd_lua_init_fun,
            NULL,
            0,
            "set lua script init function name "
            ),
    
    CH_INIT_TAKE1(
            "CHLuaFinFun",
            cmd_lua_fin_fun,
            NULL,
            0,
            "set lua script fin function name"
            ),
    
    CH_INIT_TAKE3(
            "CHLuaScript",
            cmd_lua_script,
            NULL,
            0,
            "set lua script"
            )
};

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


static int init_lua_vm(ch_lua_filter_engine_t *engine){
    
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
    
    engine->state = L;
    
    return 0;
}

ch_lua_filter_engine_t * ch_lua_filter_engine_create(ch_pool_t *mp,const char *cfname){

	const char *msg;
    int rc;
    ch_lua_script_t *srpt;

    ch_lua_filter_engine_t *engine = (ch_lua_filter_engine_t*)ch_palloc(mp,sizeof(ch_lua_filter_engine_t));

    engine->mp = mp;

    engine->lua_path = NULL;
    engine->lua_cpath = NULL;
	engine->lua_init_fun = NULL;
	engine->lua_run_fun = NULL;
	engine->lua_fin_fun = NULL;

    INIT_LIST_HEAD(&engine->scripts);

    /*run lua engine config */
	msg = ch_process_command_config(lua_directives,(void*)engine,mp,mp,cfname);

    /*meet error*/
    if(msg!=NULL){
        
		ch_log(CH_LOG_ERR,"Config Lua filter error:%s",msg);
		return NULL;
    }
    
    /*init lua state(vm)*/

    rc = init_lua_vm(engine);

    if(rc!=0){
		ch_log(CH_LOG_ERR,"Init lua state failed!");
        return NULL;
    }

    /*compile lua scripts*/
    list_for_each_entry(srpt,&engine->scripts,node){
        rc = ch_lua_script_compile(engine->state,srpt);
        if(rc!=0){
			ch_log(CH_LOG_ERR,"Compile the lua script:%s failed!",srpt->key);
            return NULL;
        }
    }

    return engine;
}

ch_lua_script_t* ch_lua_filter_engine_script_find(ch_lua_filter_engine_t *engine,const char *key){

	ch_lua_script_t *srpt = NULL;
    
	list_for_each_entry(srpt,&engine->scripts,node){
	
		if(strcmp(key,srpt->key)== 0)
			return srpt;
	}

	return NULL;
}

int ch_lua_filter_engine_script_run(ch_lua_filter_engine_t *engine,ch_lua_script_t *scrpt,ch_packet_t *pkt){

	return ch_lua_script_run(engine->state,pkt,scrpt,(void*)engine);
}

int ch_lua_filter_engine_script_run_by_key(ch_lua_filter_engine_t *engine,const char *key,ch_packet_t *pkt){

	ch_lua_script_t *srpt = ch_lua_filter_engine_script_find(engine,key);
	if(srpt == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot find lua script:%s",key);
		return -1;
	}

	return ch_lua_script_run(engine->state,pkt,srpt,(void*)engine);
}

void ch_lua_filter_engine_exit(ch_lua_filter_engine_t *engine ch_unused){

}
