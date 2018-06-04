/*
 * =====================================================================================
 *
 *       Filename:  ch_session_format_msgpack.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/18/2016 07:28:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jianfeng sha), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_session_format_msgpack.h"

static int msgpack_format(ch_session_format_t *fmt,ch_session_entry_t *sentry,
	void *session,ch_session_format_result_t *result,void *priv_data){

    char ipb[32];

    ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;

	msgpack_packer *pk = &msgpack_fmt->pk;

	/*packer the common data into msgpack*/
	ch_msgpack_map_start(pk,NULL,2);
	ch_msgpack_map_start(pk,"common",7);
	ch_msgpack_write_uint64(pk,"sessionID",sentry->session_id);
	ch_msgpack_write_uint64(pk,"reqTime",sentry->req_time);
	ch_msgpack_write_uint64(pk,"resTime",sentry->res_time);
	ch_msgpack_write_kv(pk,"srcIP",ch_ipaddr_str_get(ipb,32,sentry->src_ip));
	ch_msgpack_write_kv(pk,"dstIP",ch_ipaddr_str_get(ipb,32,sentry->dst_ip));
	ch_msgpack_write_uint16(pk,"srcPort",sentry->src_port);
	ch_msgpack_write_uint16(pk,"dstPort",sentry->dst_port);

	fmt->format2(fmt,session,priv_data);

    result->data = msgpack_fmt->pk_buf.data;
    result->dlen = msgpack_fmt->pk_buf.size;

    msgpack_sbuffer_clear(&msgpack_fmt->pk_buf);

    return 0;
}

ch_session_format_t * ch_session_format_msgpack_create(apr_pool_t *mp,
	    ch_file_store_t *fstore){


    ch_session_format_msgpack_t *msgpack_fmt = NULL;

    msgpack_fmt = apr_palloc(mp,sizeof(*msgpack_fmt));

    ch_session_format_init(&msgpack_fmt->fmt,fstore,msgpack_format);


    msgpack_sbuffer_init(&msgpack_fmt->pk_buf);
    msgpack_packer_init(&msgpack_fmt->pk,&msgpack_fmt->pk_buf,msgpack_sbuffer_write);

    return (ch_session_format_t*)msgpack_fmt;
}

void ch_session_format_msgpack_destroy(ch_session_format_t *fmt){

    ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;

    msgpack_sbuffer_destroy(&msgpack_fmt->pk_buf);
}
