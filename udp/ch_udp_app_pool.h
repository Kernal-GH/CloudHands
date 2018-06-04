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

#include "ch_mpool.h"
#include "ch_tables.h"
#include "ch_udp_session.h"
#include "ch_data_output.h"
#include "ch_packet_udp.h"

/*UDP APP Packet Process Status*/
#define PROCESS_CONTINUE 0
#define PROCESS_DONE 1
#define PROCESS_ERR 2

/*UDP APP Protocols*/
#define UDP_APP_DNS 1
#define UDP_APP_TFTP 2

struct ch_udp_app_pool_t {

	ch_pool_t *mp;
	ch_array_header_t *apps;

};

struct ch_udp_app_t {

	uint8_t type;

	int (*is_accept)(ch_packet_udp_t *pkt_udp,void *priv_data);
	
	int (*is_request)(ch_packet_udp_t *pkt_udp,void *priv_data);

	void * (*app_session_create)(void *priv_data);

	int (*pkt_process)(void *app_session,ch_packet_udp_t *pkt_udp,void *priv_data);

	ssize_t (*app_session_write)(ch_data_output_t *dout,void *app_session,void *priv_data);
	
	void (*app_session_dump)(FILE *fp,void *app_session,void *priv_data);

	void (*app_session_fin)(void *app_session,void *priv_data);
};

static inline void ch_udp_app_register(ch_udp_app_pool_t *app_pool,ch_udp_app_t *app){


	*(ch_udp_app_t**)ch_array_push(app_pool->apps) = app;

}

static inline ch_udp_app_t *ch_udp_app_find(ch_udp_app_pool_t *app_pool,ch_packet_udp_t *pkt_udp){

	int i;
	ch_udp_app_t **apps,*app = NULL;

	apps = (ch_udp_app_t**)app_pool->apps->elts;
	
	for(i = 0;i<app_pool->apps->nelts;i++){
	
		app = apps[i];

		if(app->is_accept(pkt_udp,NULL))
			return app;
	}


	return NULL;
}

extern ch_udp_app_pool_t * ch_udp_app_pool_create(ch_pool_t *mp);

extern void* ch_udp_app_session_create(ch_udp_app_t *uapp,void *priv_data);

extern int  ch_udp_app_packet_process(ch_udp_app_t *uapp,ch_udp_session_t *usession,ch_packet_udp_t *pkt_udp,void *priv_data);

extern ssize_t  ch_udp_app_session_write(ch_udp_app_t *uapp,ch_data_output_t *dout,ch_udp_session_t *usession,void *priv_data);

extern void ch_udp_app_session_dump(ch_udp_app_t *uapp,FILE *fp,ch_udp_session_t *udp_session,void *priv_data);

extern void ch_udp_app_session_fin(ch_udp_app_t *uapp,ch_udp_session_t *usession,void *priv_data);

#define ch_udp_app_session_is_request(app,pkt) (app->is_request(pkt,NULL))

#endif /*CH_UDP_APP_POOL_H*/
