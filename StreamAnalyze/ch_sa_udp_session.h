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
#include "ch_sa_data_store_pool.h"

struct ch_sa_udp_session_endpoint_t {

	ch_sa_data_store_t *dstore;
	
	uint16_t port;
	uint32_t ip;
    uint8_t addr[16];

	uint64_t packets;
	uint64_t bytes;
	
	uint64_t start_time;
	uint64_t last_time;

	int error;
};

struct ch_sa_udp_session_t {

	ch_ptable_entry_t entry;

    uint8_t is_ipv6;

	ch_sa_udp_session_endpoint_t endpoint_req;
	ch_sa_udp_session_endpoint_t endpoint_res;

	uint64_t session_id;

};

#define ch_sa_udp_session_srcport_get(sa_udp_session) ((sa_udp_session)->endpoint_req.port)
#define ch_sa_udp_session_dstport_get(sa_udp_session) ((sa_udp_session)->endpoint_res.port)
#define ch_sa_udp_session_srcip_get(sa_udp_session)   ((sa_udp_session)->endpoint_req.ip)
#define ch_sa_udp_session_dstip_get(sa_udp_session)   ((sa_udp_session)->endpoint_res.ip)
#define ch_sa_udp_session_srcaddr_get(sa_udp_session)   ((sa_udp_session)->endpoint_req.addr)
#define ch_sa_udp_session_dstaddr_get(sa_udp_session)   ((sa_udp_session)->endpoint_res.addr)

#define ch_sa_udp_session_req_packets(sa_udp_session) ((sa_udp_session)->endpoint_req.packets)
#define ch_sa_udp_session_req_bytes(sa_udp_session) ((sa_udp_session)->endpoint_req.bytes)
#define ch_sa_udp_session_req_start_time(sa_udp_session) ((sa_udp_session)->endpoint_req.start_time)
#define ch_sa_udp_session_req_last_time(sa_udp_session) ((sa_udp_session)->endpoint_req.last_time)

#define ch_sa_udp_session_res_packets(sa_udp_session) ((sa_udp_session)->endpoint_res.packets)
#define ch_sa_udp_session_res_bytes(sa_udp_session) ((sa_udp_session)->endpoint_res.bytes)
#define ch_sa_udp_session_res_start_time(sa_udp_session) ((sa_udp_session)->endpoint_res.start_time)
#define ch_sa_udp_session_res_last_time(sa_udp_session) ((sa_udp_session)->endpoint_res.last_time)
#define ch_sa_udp_session_isipv6(sa_udp_session) ((sa_udp_session)->is_ipv6)



#define ch_sa_udp_session_dstore_free(usession) do {				\
	ch_sa_data_store_t *rq_dstore = usession->endpoint_req.dstore;	\
	ch_sa_data_store_t *rs_dstore = usession->endpoint_res.dstore;	\
	if(rq_dstore)													\
		ch_sa_data_store_free(rq_dstore);							\
	if(rs_dstore)													\
		ch_sa_data_store_free(rs_dstore);							\
	usession->endpoint_req.dstore = NULL;							\
	usession->endpoint_res.dstore = NULL;							\
}while(0)

static inline int is_udp_request(ch_sa_udp_session_t *udp_session,ch_packet_udp_t *pkt){

    ch_sa_udp_session_endpoint_t *req = &udp_session->endpoint_req;
    ch_sa_udp_session_endpoint_t *res = &udp_session->endpoint_res;
    
    if(pkt->is_ipv6){

        return (udp_session->is_ipv6)&&(memcmp(req->addr,pkt->src_addr,16)==0)&&(req->port==pkt->src_port)&&(memcmp(res->addr,pkt->dst_addr,16)==0)&&(res->port==pkt->dst_port);
    }else
    {
        return (req->ip==pkt->src_ip)&&(req->port == pkt->src_port)&&(res->ip==pkt->dst_ip)&&(res->port==pkt->dst_port);
    }
}

static inline int is_udp_response(ch_sa_udp_session_t *udp_session,ch_packet_udp_t *pkt){

    ch_sa_udp_session_endpoint_t *req = &udp_session->endpoint_req;
    ch_sa_udp_session_endpoint_t *res = &udp_session->endpoint_res;
    
    if(pkt->is_ipv6){

        return (udp_session->is_ipv6)&&(memcmp(req->addr,pkt->dst_addr,16)==0)&&(req->port==pkt->dst_port)&&(memcmp(res->addr,pkt->src_addr,16)==0)&&(res->port==pkt->src_port);
    }else
    {
        return (req->ip==pkt->dst_ip)&&(req->port == pkt->dst_port)&&(res->ip==pkt->src_ip)&&(res->port==pkt->src_port);
    }
}


static inline uint32_t ch_sa_udp_session_req_data_size(ch_sa_udp_session_t *udp_session) {

	ch_sa_data_store_t *dstore = udp_session->endpoint_req.dstore;
	if(dstore == NULL)
		return 0;

	return ch_sa_data_store_size(dstore);
}

static inline uint32_t ch_sa_udp_session_res_data_size(ch_sa_udp_session_t *udp_session) {

	ch_sa_data_store_t *dstore = udp_session->endpoint_res.dstore;
	if(dstore == NULL)
		return 0;

	return ch_sa_data_store_size(dstore);
}

static inline void ch_sa_udp_session_init(ch_sa_udp_session_t *udp_session,ch_packet_udp_t *pkt_udp,uint64_t session_id){

    ch_sa_udp_session_endpoint_t *req = &udp_session->endpoint_req;
    ch_sa_udp_session_endpoint_t *res = &udp_session->endpoint_res;

	uint64_t time = ch_get_current_timems();

	udp_session->session_id = session_id;
    udp_session->is_ipv6 = pkt_udp->is_ipv6;

	/*init req endpoint*/
    if(pkt_udp->is_ipv6){

        memcpy(req->addr,pkt_udp->src_addr,16);
        memcpy(res->addr,pkt_udp->dst_addr,16);

    }else{
    
        req->ip = pkt_udp->src_ip;
        res->ip = pkt_udp->dst_ip;
    }

	req->port = pkt_udp->src_port;
	req->packets = 1;
	req->bytes = pkt_udp->payload_len;
	req->start_time = time;
	req->last_time = time;
	req->dstore = NULL;
	req->error = 0;

	/*init res endpoint*/
	res->port = pkt_udp->dst_port;
	res->packets = 0;
	res->bytes = 0;
	res->start_time = time;
	res->last_time = time;
	res->dstore = NULL;
	res->error = 0;
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

    if(is_udp_request(sa_udp_session,sa_udp_pkt)){
        /*request packet!*/
        return req;
    }

    if(is_udp_response(sa_udp_session,sa_udp_pkt)){
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
