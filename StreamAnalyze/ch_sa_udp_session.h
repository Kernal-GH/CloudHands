/*
 *
 *      Filename: ch_sa_udp_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 11:27:33
 * Last Modified: 2018-04-13 11:27:33
 */

#ifndef CH_SA_UDP_SESSION_H
#define CH_SA_UDP_SESSION_H


typedef struct ch_sa_udp_session_t ch_sa_udp_session_t;
typedef struct ch_sa_udp_session_endpoint_t ch_sa_udp_session_endpoint_t;

#include "ch_ptable.h"
#include "ch_packet_udp.h"
#include "ch_util.h"
#include "ch_rdb_store.h"

struct ch_sa_udp_session_endpoint_t {

	uint16_t port;
	uint32_t ip;

	uint32_t data_size;
	uint32_t data_seq;
	
	uint64_t packets;
	uint64_t bytes;
	
	uint64_t start_time;
	uint64_t last_time;

};

struct ch_sa_udp_session_t {

	ch_ptable_entry_t entry;

	ch_sa_udp_session_endpoint_t endpoint_req;
	ch_sa_udp_session_endpoint_t endpoint_res;

	ch_rdb_store_t *rdb_store;

	uint64_t session_id;

};

#define ch_sa_udp_session_srcport_get(sa_udp_session) ((sa_udp_session)->endpoint_req.port)
#define ch_sa_udp_session_dstport_get(sa_udp_session) ((sa_udp_session)->endpoint_res.port)
#define ch_sa_udp_session_srcip_get(sa_udp_session)   ((sa_udp_session)->endpoint_req.ip)
#define ch_sa_udp_session_dstip_get(sa_udp_session)   ((sa_udp_session)->endpoint_res.ip)

#define ch_sa_udp_session_req_packets(sa_udp_session) ((sa_udp_session)->endpoint_req.packets)
#define ch_sa_udp_session_req_bytes(sa_udp_session) ((sa_udp_session)->endpoint_req.bytes)
#define ch_sa_udp_session_req_start_time(sa_udp_session) ((sa_udp_session)->endpoint_req.start_time)
#define ch_sa_udp_session_req_last_time(sa_udp_session) ((sa_udp_session)->endpoint_req.last_time)
#define ch_sa_udp_session_req_data_size(sa_udp_session) ((sa_udp_session)->endpoint_req.data_size)
#define ch_sa_udp_session_req_data_seq(sa_udp_session) ((sa_udp_session)->endpoint_req.data_seq)

#define ch_sa_udp_session_res_packets(sa_udp_session) ((sa_udp_session)->endpoint_res.packets)
#define ch_sa_udp_session_res_bytes(sa_udp_session) ((sa_udp_session)->endpoint_res.bytes)
#define ch_sa_udp_session_res_start_time(sa_udp_session) ((sa_udp_session)->endpoint_res.start_time)
#define ch_sa_udp_session_res_last_time(sa_udp_session) ((sa_udp_session)->endpoint_res.last_time)
#define ch_sa_udp_session_res_data_size(sa_udp_session) ((sa_udp_session)->endpoint_res.data_size)
#define ch_sa_udp_session_res_data_seq(sa_udp_session) ((sa_udp_session)->endpoint_res.data_seq)

#define EP_EQUAL(ep,eip,eport) ((ep->ip == eip)&&(ep->port == eport))

static inline void ch_sa_udp_session_init(ch_sa_udp_session_t *udp_session,ch_packet_udp_t *pkt_udp,uint64_t session_id){

    ch_sa_udp_session_endpoint_t *req = &udp_session->endpoint_req;
    ch_sa_udp_session_endpoint_t *res = &udp_session->endpoint_res;

	uint64_t time = ch_get_current_timems();

	udp_session->session_id = session_id;

	/*init req endpoint*/
	req->ip = pkt_udp->src_ip;
	req->port = pkt_udp->src_port;
	req->data_size = 0;
	req->data_seq = 0;
	req->packets = 1;
	req->bytes = pkt_udp->payload_len;
	req->start_time = time;
	req->last_time = time;

	/*init res endpoint*/
	res->ip = pkt_udp->dst_ip;
	res->port = pkt_udp->dst_port;
	res->data_size = 0;
	res->data_seq = 0;
	res->packets = 0;
	res->bytes = 0;
	res->start_time = time;
	res->last_time = time;

}

static inline void 
ch_sa_udp_session_update(ch_sa_udp_session_t *udp_session ch_unused,ch_sa_udp_session_endpoint_t *ep,ch_packet_udp_t *udp_pkt){


	uint64_t time = ch_get_current_timems();
	ep->packets += 1;
	ep->bytes += udp_pkt->payload_len;

	if(ep->packets == 0){
	
		ep->start_time = time;
	}

	ep->last_time = time;

}

static inline ch_sa_udp_session_endpoint_t * 
ch_sa_udp_session_endpoint_get(ch_sa_udp_session_t *sa_udp_session,ch_packet_udp_t *sa_udp_pkt){
   
    ch_sa_udp_session_endpoint_t *req = &sa_udp_session->endpoint_req;
    ch_sa_udp_session_endpoint_t *res = &sa_udp_session->endpoint_res;

    if(EP_EQUAL(req,sa_udp_pkt->src_ip,sa_udp_pkt->src_port)&&EP_EQUAL(res,sa_udp_pkt->dst_ip,sa_udp_pkt->dst_port)){
        /*request packet!*/
        return req;
    }

    if(EP_EQUAL(req,sa_udp_pkt->dst_ip,sa_udp_pkt->dst_port)&&EP_EQUAL(res,sa_udp_pkt->src_ip,sa_udp_pkt->src_port)){
        /*response packet!*/
        return res;
    }

    /*no belong this session!*/
    return NULL;
}

static inline ch_sa_udp_session_endpoint_t * 
ch_sa_udp_session_pair_endpoint_get(ch_sa_udp_session_t *sa_udp_session,ch_packet_udp_t *sa_udp_pkt){

    ch_sa_udp_session_endpoint_t *ep = NULL;
    ch_sa_udp_session_endpoint_t *req = &sa_udp_session->endpoint_req;
    ch_sa_udp_session_endpoint_t *res = &sa_udp_session->endpoint_res;

	ep = ch_sa_udp_session_endpoint_get(sa_udp_session,sa_udp_pkt);

	if(ep){
	
		return ep == req?res:req;
	}

	return NULL;
}

#endif /*CH_SA_UDP_SESSION_H*/
