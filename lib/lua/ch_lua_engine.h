/*
 * =====================================================================================
 *
 *       Filename:  ch_lua_engine.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/12/2018 11:23:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_LUA_ENGINE_H
#define CH_LUA_ENGINE_H

typedef struct ch_lua_engine_t ch_lua_engine_t;

#include "ch_lua.h"
#include "ch_mpool.h"
#include "ch_tables.h"


struct ch_lua_engine_t {

    lua_State *lua_state;

    ch_pool_t *mp;

    const char *lua_path;
    const char *lua_cpath;

    const char *lua_init_fun;
    const char *lua_run_fun;
    const char *lua_timeout_fun;
    const char *lua_fin_fun;

    int is_cache;

    const char *lua_fname;

    ch_array_header_t *chunk_parts;

    uint64_t timeout;
    uint64_t last_time;

};

extern ch_lua_engine_t * ch_lua_engine_create(ch_pool_t *mp,const char *lua_path,
        const char *lua_cpath,
        const char *lua_init_fun,
        const char *lua_run_fun,
        const char *lua_timeout_fun,
        const char *lua_fin_fun,
        int is_cache,
        const char *lua_fname,
        const char *data_key,
        void *data,
        uint64_t timeout);


extern int ch_lua_engine_run(ch_lua_engine_t *engine,const char *idata_key,void *idata,
        const char *odata_key,void *odata);


extern int ch_lua_engine_timeout(ch_lua_engine_t *engine,const char *idata_key,void *idata,
        const char *odata_key,void *odata);


extern void ch_lua_engine_fin(ch_lua_engine_t *engine);

#define ch_lua_engine_udata_set(lua_engine,key,data) do { \
    lua_pushlightuserdata(lua_engine->lua_state,data);    \
    lua_setglobal(lua_engine->lua_state, key);            \
}while(0)

#endif /* CH_LUA_ENGINE_H */
