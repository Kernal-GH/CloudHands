/*
 *
 *      Filename: ch_udp_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-19 16:36:56
 * Last Modified: 2018-04-19 16:36:56
 */

#ifndef CH_UDP_SESSION_H
#define CH_UDP_SESSION_H

typedef struct ch_udp_session_t ch_udp_session_t;
typedef struct ch_udp_session_endpoint_t ch_udp_session_endpoint_t;

#include "ch_ptable.h"
#include "ch_packet_udp.h"
#include "ch_util.h"
#include "ch_udp_app_pool.h"

struct ch_udp_session_endpoint_t {

	uint16_t port;
	uint32_t ip;

	uint64_t packets;
	uint64_t bytes;
	
	uint64_t start_time;
	uint64_t last_time;

};


struct ch_udp_session_t {

	ch_ptable_entry_t entry;
	
	ch_udp_session_endpoint_t endpoint_req;
	ch_udp_session_endpoint_t endpoint_res;

	uint64_t session_id;
	
	ch_udp_app_t *app;

	void *app_session;
};

#define ch_udp_session_srcport_get(udp_session) ((udp_session)->endpoint_req.port)
#define ch_udp_session_dstport_get(udp_session) ((udp_session)->endpoint_res.port)
#define ch_udp_session_srcip_get(udp_session)   ((udp_session)->endpoint_req.ip)
#define ch_udp_session_dstip_get(udp_session)   ((udp_session)->endpoint_res.ip)

#define ch_udp_session_req_packets(udp_session) ((udp_session)->endpoint_req.packets)
#define ch_udp_session_req_bytes(udp_session) ((udp_session)->endpoint_req.bytes)
#define ch_udp_session_req_start_time(udp_session) ((udp_session)->endpoint_req.start_time)
#define ch_udp_session_req_last_time(udp_session) ((udp_session)->endpoint_req.last_time)

#define ch_udp_session_res_packets(udp_session) ((udp_session)->endpoint_res.packets)
#define ch_udp_session_res_bytes(udp_session) ((udp_session)->endpoint_res.bytes)
#define ch_udp_session_res_start_time(udp_session) ((udp_session)->endpoint_res.start_time)
#define ch_udp_session_res_last_time(udp_session) ((udp_session)->endpoint_res.last_time)

#define EP_EQUAL(ep,eip,eport) ((ep->ip == eip)&&(ep->port == eport))


extern int ch_udp_session_init(ch_udp_session_t *udp_session,ch_packet_udp_t *pkt_udp,ch_udp_app_t *app,
	uint64_t session_id);

static inline void 
ch_udp_session_update(ch_udp_session_t *udp_session ch_unused,ch_udp_session_endpoint_t *ep,ch_packet_udp_t *udp_pkt){


	uint64_t time = ch_get_current_timems();
	ep->packets += 1;
	ep->bytes += udp_pkt->payload_len;

	if(ep->packets == 0){
	
		ep->start_time = time;
	}

	ep->last_time = time;

}

static inline ch_udp_session_endpoint_t * 
ch_udp_session_endpoint_get(ch_udp_session_t *udp_session,ch_packet_udp_t *udp_pkt){
   
    ch_udp_session_endpoint_t *req = &udp_session->endpoint_req;
    ch_udp_session_endpoint_t *res = &udp_session->endpoint_res;

    if(EP_EQUAL(req,udp_pkt->src_ip,udp_pkt->src_port)&&EP_EQUAL(res,udp_pkt->dst_ip,udp_pkt->dst_port)){
        /*request packet!*/
        return req;
    }

    if(EP_EQUAL(req,udp_pkt->dst_ip,udp_pkt->dst_port)&&EP_EQUAL(res,udp_pkt->src_ip,udp_pkt->src_port)){
        /*response packet!*/
        return res;
    }

    /*no belong this session!*/
    return NULL;
}

static inline ch_udp_session_endpoint_t * 
ch_udp_session_pair_endpoint_get(ch_udp_session_t *udp_session,ch_packet_udp_t *udp_pkt){

    ch_udp_session_endpoint_t *ep = NULL;
    ch_udp_session_endpoint_t *req = &udp_session->endpoint_req;
    ch_udp_session_endpoint_t *res = &udp_session->endpoint_res;

	ep = ch_udp_session_endpoint_get(udp_session,udp_pkt);

	if(ep){
	
		return ep == req?res:req;
	}

	return NULL;
}

#endif /*CH_UDP_SESSION_H*/
