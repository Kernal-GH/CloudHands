/*
 *
 *      Filename: ch_lua_packet_api.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-11 15:58:44
 * Last Modified: 2017-11-11 15:58:44
 */

#ifndef CH_LUA_PACKET_API_H
#define CH_LUA_PACKET_API_H

#include "ch_lua.h"
#include "ch_packet.h"

#define LUA_FFI_OK 0
#define LUA_FFI_ERROR -1

typedef struct {

    int len;
    char *data;
}ch_ffi_str_t;


static inline void ch_lua_packet_api_set(lua_State *L, ch_packet_t *pkt)
{
    lua_pushlightuserdata(L, pkt);
    lua_setglobal(L, "__ch_packet");
}

#endif /*CH_LUA_PACKET_API_H*/
