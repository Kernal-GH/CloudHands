/*
 * =====================================================================================
 *
 *       Filename:  ch_ftp_stat_lua_api.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/06/2019 02:32:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_FTP_STAT_LUA_API_H
#define CH_FTP_STAT_LUA_API_H

typedef struct ch_ftp_stat_data_t ch_ftp_stat_data_t;
typedef struct ch_ftp_stat_store_t ch_ftp_stat_store_t;

#include <msgpack.h>
#include "ch_shm_format.h"

struct ch_ftp_stat_data_t {

    const char *key;
    uint64_t count;
};

struct ch_ftp_stat_store_t {

    ch_shm_fmt_t *shm_fmt;
    msgpack_packer pk;
    msgpack_sbuffer pk_buf;
};


#endif /* CH_FTP_STAT_LUA_API_H */
