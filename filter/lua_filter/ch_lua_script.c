/*shajf*/
#include "ch_log.h"
#include "ch_constants.h"
#include "ch_lua.h"
#include "ch_lua_script.h"

typedef struct {
    
    const void *data;
    size_t len;
}lua_chunk_part_t;

typedef struct {
    ch_lua_script_t *srpt;
    int index;
}lua_load_srpt_t;

static int lua_loadfrom_file(lua_State *state,ch_lua_script_t *srpt);

static const char* 
load_reader(lua_State* L ch_unused, void* user_data, size_t* size) 
{
	lua_load_srpt_t *load_srpt = (lua_load_srpt_t *)user_data;
	lua_chunk_part_t *part;
	ch_lua_script_t *srpt =load_srpt->srpt;

	/* Do we have more chunks to return? */
	if (load_srpt->index ==srpt->chunk_parts->nelts) 
	{
		return NULL;
	}

	/* Get one chunk. */
	part = ((lua_chunk_part_t **)srpt->chunk_parts->elts)[load_srpt->index];
   
    *size = part->len;

	load_srpt->index++;

	return (const char *)part->data;
}

static int dump_writer_mem(lua_State * L ch_unused, const void* data, size_t len,void* user_data) 
{
	ch_lua_script_t *srpt = (ch_lua_script_t *)user_data;
	
	lua_chunk_part_t *part;
	
	void *part_data;

	part_data = ch_palloc(srpt->mp, len);
    
    memcpy(part_data, data, len);
    
	part = (lua_chunk_part_t*)ch_palloc(srpt->mp,sizeof(lua_chunk_part_t));

	part->data = part_data;
	part->len = len;

	*(const lua_chunk_part_t **)ch_array_push(srpt->chunk_parts) =part;

	return 0;
}

ch_lua_script_t * ch_lua_script_create(ch_pool_t *mp,ch_lua_script_ops_t *ops,
        const char *key,uint64_t id,const char * lua_fname,int is_cache){

    ch_lua_script_t *srpt = (ch_lua_script_t*)ch_palloc(mp,sizeof(ch_lua_script_t));

    srpt->ops = ops;
    srpt->mp = mp;
	srpt->key = key;
    srpt->id = id;
    srpt->lua_fname = lua_fname;
    srpt->load_state = LOAD_NO;
    srpt->is_cache = is_cache;

    return srpt;

}

int ch_lua_script_compile(lua_State *state,ch_lua_script_t *srpt){

    int rc;

    /*if is_cache == 1,then lua script will be compile and write into memory,
     * this will be faster when exe it*/

    if(srpt->is_cache == 1){

        /*create a array to cache lua scipt*/
        srpt->chunk_parts = ch_array_make(srpt->mp,32,sizeof(lua_chunk_part_t*));

        if(srpt->chunk_parts == NULL){
            return -1;
        }

        rc = lua_loadfrom_file(state,srpt);

        if(rc!=0){
			ch_log(CH_LOG_ERR,"Cannot load lua script from file:%s",srpt->lua_fname);
            return -1;
        }

        rc = lua_dump(state,dump_writer_mem,(void*)srpt);

        /*failed*/
        if(rc!=0){
            char *errmsg;
            size_t len;

            /*get error msg from lua stack top*/
            ch_lua_stack_get_string(state,&errmsg,&len,-1);
            if(errmsg){
                ch_log(CH_LOG_ERR,"dump lua stript===%s failed from file  [%s]",srpt->lua_fname,errmsg);

            }
            return -1;
        }
    }else{
        srpt->chunk_parts = NULL;
    }

    return 0;
}

static int lua_loadfrom_cache(lua_State *state,ch_lua_script_t *srpt){

    int rc;

    lua_load_srpt_t load_srpt;
    load_srpt.srpt = srpt;
    load_srpt.index = 0;

    rc = lua_load(state,load_reader,(void*)&load_srpt,srpt->lua_fname);
    
    /*load failed*/
    if(rc!=LUA_OK){

        char *errmsg;
        size_t len;

        /*get error msg from lua stack top*/
        ch_lua_stack_get_string(state,&errmsg,&len,-1);
        if(errmsg){
            ch_log(CH_LOG_ERR,"Load lua stript===%s failed from memory cache [%s]",srpt->lua_fname,errmsg);

        }

        return -1; 
    }

    return 0;
}

static int lua_loadfrom_file(lua_State *state,ch_lua_script_t *srpt){
    
    int rc;
    
    rc = luaL_loadfile(state,srpt->lua_fname);
    
    /*load failed*/
    if(rc!=LUA_OK){

        char *errmsg;
        size_t len;

        /*get error msg from lua stack top*/
        ch_lua_stack_get_string(state,&errmsg,&len,-1);
        if(errmsg){
			ch_log(CH_LOG_ERR,"Load lua stript===%s failed from file  [%s]",srpt->lua_fname,errmsg);

        }

        return -1; 
    }

    return 0;
}

int ch_lua_script_load(lua_State *state,ch_lua_script_t *srpt){

    int rc;

    /*load from cache*/
    if(srpt->chunk_parts){

        rc = lua_loadfrom_cache(state,srpt);
    }else{
        /*load from file*/
        rc = lua_loadfrom_file(state,srpt);
    }

    srpt->load_state = rc==0?LOAD_OK:LOAD_FAILED;

    return rc;
} 

int ch_lua_script_run(lua_State *state,ch_packet_t *packet,ch_lua_script_t *srpt,void *user_data){

    int rc;
    size_t len;
    char *errmsg;

    if(srpt->load_state == LOAD_NO){
        ch_lua_script_load(state,srpt);
        /*here,lua script load ok,can be run*/
        rc = lua_pcall(state,0,0,0);
        
        if(rc!=LUA_OK){

            /*get error msg from lua stack top*/
            ch_lua_stack_get_string(state,&errmsg,&len,-1);
            if(errmsg){
                ch_log(CH_LOG_ERR,"run lua stript===%s failed  [%s]",srpt->lua_fname,errmsg);
            }

            return -1;
        }

        if(srpt->ops){

            if(srpt->ops->lua_run_init){
                rc = srpt->ops->lua_run_init(state,user_data);
                if(rc!=0){
                    ch_log(CH_LOG_ERR,"run lua:%s init function failed!",srpt->lua_fname);
                    return -1;
                }
            }
        }
    }

    if(srpt->load_state==LOAD_FAILED){
        return -1;
    }
    
    /*now can call some lua function to complete works*/

    if(srpt->ops){

        if(srpt->ops->lua_run){
            rc = srpt->ops->lua_run(state,packet,user_data);
            if(rc==-1){
                ch_log(CH_LOG_ERR,"run lua run function failed,%s",srpt->lua_fname);
                if(srpt->ops->lua_run_fin){
                    srpt->ops->lua_run_fin(state,user_data);
                }
                return -1;
            }

        }
        
        if(srpt->ops->lua_run_fin){
            srpt->ops->lua_run_fin(state,user_data);
        }

    }

    return rc;
}



