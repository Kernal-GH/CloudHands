/*
 * =====================================================================================
 *
 *       Filename:  ch_ftp_stat_lua_api.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/06/2019 02:41:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_ftp_stat_lua_api.h"
#include "ch_packet_record.h"

static int ch_ftp_stat_lua_api_data_write(ch_ftp_stat_store_t *ftp_store,
        const char *key,uint64_t start_time,uint64_t last_time,uint64_t count){

    void *data;
    size_t dlen;

    msgpack_packer *pk = &ftp_store->pk;

    ch_shm_format_t *shm_fmt = ftp_store->shm_fmt;

    ch_msgpack_map_start(pk,NULL,4);

    ch_msgpack_write_uint64(pk,"startTime",start_time);
    ch_msgpack_write_uint64(pk,"endTime",last_time);

    ch_msgpack_write_kv(pk,"key",user);

    ch_msgpack_write_uint64(pk,"count",count);

    data = ftp_store->pk_buf.data;
    dlen = ftp_store->pk_buf.size;
    ch_packet_record_t pkt_rcd;

    pkt_rcd.type = PKT_RECORD_TYPE_STAT_FTP;
    pkt_rcd.meta_data_size = 0;
    pkt_rcd.time = start_time;
    ch_packet_record_put(shm_fmt,&pkt_rcd,data,dlen);

    msgpack_sbuffer_clear(&ftp_store->pk_buf);

    ch_shm_format_flush(shm_fmt);

    /* ok! */
    return 0;
}

