/*
 *
 *      Filename: ch_tcp_app_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 11:52:54
 * Last Modified: 2018-07-12 11:52:54
 */

#ifndef CH_TCP_APP_POOL_H
#define CH_TCP_APP_POOL_H

typedef struct ch_tcp_app_pool_t ch_tcp_app_pool_t;
typedef struct ch_tcp_app_t ch_tcp_app_t;

#include "ch_mpool.h"
#include "ch_tables.h"
#include "ch_tcp_app_context.h"
#include "ch_packet_tcp.h"
#include "ch_proto_session_store.h"
#include "ch_tcp_session.h"

/* Parse data status returned! */
#define PARSE_DONE 0
#define PARSE_CONTINUE 1
#define PARSE_BREAK 2

struct ch_tcp_app_pool_t {

	ch_pool_t *mp;
	ch_tcp_app_context_t *tcontext;

	ch_array_header_t *apps;

};

struct ch_tcp_app_t {

	uint32_t protocol_id;
	uint8_t pkt_rcd_type;

	void *context;

	ch_tcp_app_t* (*find_by_port)(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_packet_tcp_t *tcp_pkt);
	
	ch_tcp_app_t* (*find_by_content)(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_packet_tcp_t *tcp_pkt,void *data,size_t dlen);

	void *(*proto_session_entry_create)(ch_tcp_app_t *app,ch_proto_session_store_t *pstore);
	void  (*proto_session_entry_clean)(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession);
	void (*proto_session_format)(msgpack_packer *pk,void *session);

	int (*request_content_parse)(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,void *data,size_t dlen);
	
	int (*response_content_parse)(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,void *data,size_t dlen);

};

static inline void ch_tcp_app_register(ch_tcp_app_pool_t *ta_pool,ch_tcp_app_t *app){

	*(ch_tcp_app_t**)ch_array_push(ta_pool->apps) = app;
}



extern ch_tcp_app_pool_t * ch_tcp_app_pool_create(ch_pool_t *mp,const char *cfname);


extern ch_tcp_app_t * ch_tcp_app_find_by_port(ch_tcp_app_pool_t *ta_pool,ch_proto_session_store_t *pstore,ch_packet_tcp_t *tcp_pkt);

extern ch_tcp_app_t * ch_tcp_app_find_by_content(ch_tcp_app_pool_t *ta_pool,ch_proto_session_store_t *pstore,ch_packet_tcp_t *tcp_pkt,void *data,size_t dlen);


extern int ch_tcp_app_request_content_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
	ch_tcp_session_t *tsession,void *data,size_t dlen);


extern int ch_tcp_app_response_content_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
	 ch_tcp_session_t *tsession,void *data,size_t dlen);

extern void* ch_tcp_app_session_entry_create(ch_tcp_app_t *app,ch_proto_session_store_t *pstore);


extern void  ch_tcp_app_session_entry_clean(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession);


#endif /*CH_TCP_APP_POOL_H*/
