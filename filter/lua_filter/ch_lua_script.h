/*shajf*/
#ifndef CH_LUA_SCRIPT_H
#define CH_LUA_SCRIPT_H

#include "ch_mpool.h"
#include "ch_tables.h"
#include "ch_list.h"
#include "ch_packet.h"

typedef struct ch_lua_script_t ch_lua_script_t;
typedef struct ch_lua_script_ops_t ch_lua_script_ops_t;

#define LOAD_OK 0
#define LOAD_FAILED -1
#define LOAD_NO 1

struct  ch_lua_script_t{
    
	struct list_head node;
    uint64_t id;
    int load_state;
    int is_cache;
    ch_pool_t *mp;
    const char *lua_fname;
    const char *key;
	ch_array_header_t *chunk_parts;
    /*callback function*/
    ch_lua_script_ops_t *ops;

};

struct ch_lua_script_ops_t {
    
    /*do something before running lua script*/
    int (*lua_run_init)(lua_State *state,void *user_data);

    /*run lua script*/
    int (*lua_run)(lua_State *state,ch_packet_t *pkt,void *user_data);

    /*do something after running lua script*/
    int (*lua_run_fin)(lua_State *state,void *user_data);
};

extern ch_lua_script_t * ch_lua_script_create(ch_pool_t *mp,ch_lua_script_ops_t *ops,
	const char *key,uint64_t id,const char * lua_fname,int is_cache);

/*compile lua script,if cache set,will cache results of compiling into memory*/
extern int ch_lua_script_compile(lua_State *state,ch_lua_script_t *srpt);

    /*load lua into lua_State before running*/
extern int ch_lua_script_load(lua_State *state,ch_lua_script_t *srpt); 

/*run lua script,if lua script no loaded,will be try to load first*/
extern int ch_lua_script_run(lua_State *state,ch_packet_t *pkt,ch_lua_script_t *srpt,void *user_data);

#endif /*CH_LUA_SCRIPT_H*/
