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
#include "ch_shm_format.h"
#include "ch_tcp_session.h"

#define PARSE_RETURN_CONTINUE 0X01
#define PARSE_RETURN_DONE     0X02
#define PARSE_RETURN_DISCARD  0X04
#define PARSE_RETURN_CLOSE    0X08

struct ch_tcp_app_pool_t {

	ch_pool_t *mp;
	ch_tcp_app_context_t *tcontext;

	ch_array_header_t *apps;

};

struct ch_tcp_app_t {

	void *context;

	int (*is_accept_by_port)(ch_tcp_app_t *app,ch_packet_tcp_t *tcp_pkt);
	
	int (*is_accept_by_content)(ch_tcp_app_t *app,ch_packet_tcp_t *tcp_pkt,void *data,size_t dlen);

	int (*request_content_process)(ch_tcp_app_t *app,ch_shm_format_t *fmt,ch_tcp_session_t *tsession,void *data,size_t dlen);
	
	int (*response_content_process)(ch_tcp_app_t *app,ch_shm_format_t *fmt,ch_tcp_session_t *tsession,void *data,size_t dlen);

	void (*content_flush)(ch_tcp_app_t *app,ch_shm_format_t *fmt,ch_tcp_session_t *tsession,void *data,size_t dlen);
	
	void (*content_close)(ch_tcp_app_t *app,ch_shm_format_t *fmt,ch_tcp_session_t *tsession,void *data,size_t dlen);

};

static inline void ch_tcp_app_register(ch_tcp_app_pool_t *ta_pool,ch_tcp_app_t *app){

	*(ch_tcp_app_t**)ch_array_push(ta_pool->apps) = app;
}



extern ch_tcp_app_pool_t * ch_tcp_app_pool_create(ch_pool_t *mp,const char *cfname);


extern ch_tcp_app_t * ch_tcp_app_find_by_port(ch_tcp_app_pool_t *ta_pool,ch_packet_tcp_t *tcp_pkt);

extern ch_tcp_app_t * ch_tcp_app_find_by_content(ch_tcp_app_pool_t *ta_pool,ch_packet_tcp_t *tcp_pkt,void *data,size_t dlen);

extern int ch_tcp_app_request_content_process(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen);


extern int ch_tcp_app_response_content_process(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	 ch_tcp_session_t *tsession,void *data,size_t dlen);

extern void ch_tcp_app_content_flush(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	 ch_tcp_session_t *tsession,void *data,size_t dlen);


extern void ch_tcp_app_content_close(ch_tcp_app_t *app,ch_shm_format_t *fmt,
	 ch_tcp_session_t *tsession,void *data,size_t dlen);


#endif /*CH_TCP_APP_POOL_H*/
