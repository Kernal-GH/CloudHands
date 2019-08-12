/*
 *
 *      Filename: ch_udp_session_request.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-12 16:56:26
 * Last Modified: 2018-09-12 16:56:26
 */

#ifndef CH_UDP_SESSION_REQUEST_H
#define CH_UDP_SESSION_REQUEST_H

typedef struct ch_udp_session_request_pool_t ch_udp_session_request_pool_t;
typedef struct ch_udp_session_request_t ch_udp_session_request_t;

#include "ch_udp_work.h"
#include "ch_mpool.h"
#include "ch_packet_udp.h"
#include "ch_plist.h"
#include "ch_udp_app_pool.h"

struct ch_udp_session_request_t {

	ch_plist_entry_t entry;

	ch_udp_app_session_t *app_session;

    uint8_t is_ipv6;
	uint32_t src_ip;
	uint32_t dst_ip;
    uint8_t src_addr[16];
    uint8_t dst_addr[16];
	uint16_t src_port;
	uint16_t dst_port;

};

struct ch_udp_session_request_pool_t {


	ch_pool_t *mp;
	ch_udp_work_t *work;
	ch_plist_t *plist;

};

extern ch_udp_session_request_pool_t *ch_udp_session_request_pool_create(ch_udp_work_t *work,
	void (*entry_timeout_cb)(ch_plist_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data);


static inline ch_udp_session_request_t *
ch_udp_session_request_pool_entry_create(ch_udp_session_request_pool_t *req_pool,
	ch_packet_udp_t *udp_pkt,ch_udp_app_session_t *app_session){

	ch_udp_session_request_t *req_session;

	req_session = (ch_udp_session_request_t*)ch_plist_entry_create(req_pool->plist);

	if(req_session)
	{

		req_session->app_session = app_session;
		req_session->is_ipv6 = udp_pkt->is_ipv6;
        if(req_session->is_ipv6){
            memcpy(req_session->src_addr,udp_pkt->src_addr,16);
            memcpy(req_session->dst_addr,udp_pkt->dst_addr,16);
        }
        req_session->src_ip = udp_pkt->src_ip;
		req_session->dst_ip = udp_pkt->dst_ip;

		req_session->src_port = udp_pkt->src_port;
		req_session->dst_port = udp_pkt->dst_port;
	}

	return req_session;

}

#define ch_udp_session_request_pool_entry_find(req_pool,udp_pkt) (ch_udp_session_request_t*)ch_plist_entry_find(req_pool->plist,(void*)udp_pkt)


#define ch_udp_session_request_pool_entry_free(req_pool,req_session) ch_plist_entry_free(req_pool->plist,(ch_plist_entry_t*)req_session)


#endif /*CH_UDP_SESSION_REQUEST_H*/
