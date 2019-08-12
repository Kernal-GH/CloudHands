/*
 *
 *      Filename: ch_udp_app_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 10:27:38
 * Last Modified: 2018-05-08 10:27:38
 */

#ifndef CH_UDP_APP_POOL_H
#define CH_UDP_APP_POOL_H

typedef struct ch_udp_app_pool_t ch_udp_app_pool_t;
typedef struct ch_udp_app_t ch_udp_app_t;
typedef struct ch_udp_app_session_t ch_udp_app_session_t;

#include "ch_mpool.h"
#include "ch_tables.h"
#include "ch_udp_session.h"
#include "ch_udp_session_request_pool.h"
#include "ch_data_output.h"
#include "ch_packet_udp.h"
#include "ch_udp_app_context.h"
#include "ch_msgpack_store.h"

/*UDP APP Packet Process Status*/
#define PROCESS_CONTINUE 0
#define PROCESS_DONE 1
#define PROCESS_ERR 2

/*UDP APP Protocols*/
#define UDP_APP_DNS 1
#define UDP_APP_TFTP 2
#define UDP_APP_SMON 3

/*UDP Session Request process results*/
#define PROCESS_REQ_SESSION_DONE 0
#define PROCESS_REQ_SESSION_CONTINUE 1
#define PROCESS_REQ_SESSION_ERROR -1

struct ch_udp_app_pool_t {

	ch_pool_t *mp;

	ch_udp_app_context_t *ucontext;

	ch_array_header_t *apps;
};

struct ch_udp_app_session_t {

	ch_udp_app_t *app;

	ch_pool_t *mp;

	void *priv_data;
};

#define ch_udp_app_priv_data_get(app_session,type) (type*)((app_session)->priv_data)

struct ch_udp_app_t {

	void *context;
	

	uint8_t type;
	
	ch_udp_app_session_t * (*app_session_create)(ch_udp_app_t *app,ch_packet_udp_t *pkt_udp);
	
	int (*process_request_session)(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp);
	int (*request_session_equal)(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp);

	int (*is_request)(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp);


	int (*req_pkt_process)(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp);
	
	int (*res_pkt_process)(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp);

	ssize_t (*app_session_write)(ch_udp_app_session_t *app_session,ch_data_output_t *dout);
	
    int (*app_session_store)(ch_udp_app_session_t *app_session,ch_msgpack_store_t *dstore);
	
	void (*app_session_dump)(ch_udp_app_session_t *app_session,FILE *fp);

	void (*app_session_fin)(ch_udp_app_session_t *app_session);
};

static inline void ch_udp_app_register(ch_udp_app_pool_t *app_pool,ch_udp_app_t *app){


	*(ch_udp_app_t**)ch_array_push(app_pool->apps) = app;

}

extern ch_udp_app_pool_t * ch_udp_app_pool_create(ch_pool_t *mp,const char *cfname);

extern ch_udp_app_session_t * ch_udp_app_session_create(ch_udp_app_pool_t *app_pool, ch_packet_udp_t *pkt_udp);

extern int ch_udp_app_session_request_equal(ch_udp_session_request_t *req_session,ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp);

extern int  ch_udp_app_session_request_process(ch_udp_session_request_t *req_session,ch_udp_app_session_t *app_session,
	ch_packet_udp_t *pkt_udp);

extern int  ch_udp_app_session_packet_process(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp);

extern ssize_t  ch_udp_app_session_write(ch_udp_session_t *usession,ch_udp_app_session_t *app_session,ch_data_output_t *dout);

extern int ch_udp_app_session_store(ch_udp_session_t *usession,ch_udp_app_session_t *app_session,ch_msgpack_store_t *dstore);

extern void ch_udp_app_session_dump(ch_udp_app_session_t *app_session,ch_udp_session_t *udp_session,FILE *fp);

extern void ch_udp_app_session_fin(ch_udp_app_session_t *app_session);

#endif /*CH_UDP_APP_POOL_H*/
