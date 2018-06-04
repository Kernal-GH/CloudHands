/*
 *
 *      Filename: ch_lua_filter_engine.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-11 12:20:53
 * Last Modified: 2017-11-11 12:20:53
 */

#ifndef CH_LUA_FILTER_ENGINE_H
#define CH_LUA_FILTER_ENGINE_H

typedef struct ch_lua_filter_engine_t ch_lua_filter_engine_t;

#include "ch_mpool.h"
#include "ch_list.h"
#include "ch_lua.h"
#include "ch_lua_script.h"

struct ch_lua_filter_engine_t {

   struct list_head scripts;
   ch_pool_t *mp;
   lua_State *state;

   const char * lua_path;
   const char * lua_cpath;

   const char *lua_init_fun;
   const char *lua_run_fun;
   const char *lua_fin_fun;

};

extern ch_lua_filter_engine_t* ch_lua_filter_engine_create(ch_pool_t *mp,const char *cfname);

extern ch_lua_script_t* ch_lua_filter_engine_script_find(ch_lua_filter_engine_t *engine,const char *key);

extern int ch_lua_filter_engine_script_run(ch_lua_filter_engine_t *engine,ch_lua_script_t *scrpt,ch_packet_t *pkt);

extern int ch_lua_filter_engine_script_run_by_key(ch_lua_filter_engine_t *engine,const char *key,ch_packet_t *pkt);

extern void ch_lua_filter_engine_exit(ch_lua_filter_engine_t *engine);

#endif /*CH_LUA_FILTER_ENGINE_H*/
