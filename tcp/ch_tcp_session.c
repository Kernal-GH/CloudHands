/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月30日 14时51分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 */

#include "ch_tcp_session.h"

int ch_tcp_session_init(ch_tcp_session_t *tcp_session,ch_tcp_session_request_t *sreq,ch_tcp_app_t *app,
        void *sentry,
        ch_memory_t *mm){

    if(sreq->is_ipv6){

        ch_tcp_session_endpoint_init_ipv6(&tcp_session->endpoint_req,sreq->src_addr,sreq->req_port,sreq->req_sn_init,mm);
        ch_tcp_session_endpoint_init_ipv6(&tcp_session->endpoint_res,sreq->dst_addr,sreq->res_port,sreq->res_sn_init,mm);
    }else{
        ch_tcp_session_endpoint_init(&tcp_session->endpoint_req,sreq->req_ip,sreq->req_port,sreq->req_sn_init,mm);
        ch_tcp_session_endpoint_init(&tcp_session->endpoint_res,sreq->res_ip,sreq->res_port,sreq->res_sn_init,mm);
    }

    tcp_session->state = SESSION_STATE_INIT; 

    tcp_session->need_end = 0;
    tcp_session->fin1_seq = 0;
    tcp_session->fin2_seq = 0;
    tcp_session->four_way_state = FOUR_WAY_INIT;

    tcp_session->cur_ep = NULL;
    tcp_session->app = app;
    tcp_session->sentry = sentry;
    return 0;
}

void ch_tcp_session_fin(ch_tcp_session_t *tcp_session){

    ch_tcp_session_endpoint_fin(&tcp_session->endpoint_req);
    ch_tcp_session_endpoint_fin(&tcp_session->endpoint_res);
}

static inline int _endpoint_is_request(ch_tcp_session_endpoint_t *req,ch_tcp_session_endpoint_t *res,ch_packet_tcp_t *tcp_pkt){

    if(tcp_pkt->is_ipv6){
        return (req->is_ipv6)&&(memcmp(req->addr,tcp_pkt->src_addr,16)==0)&&(req->port==tcp_pkt->src_port)&&(res->is_ipv6)&&(memcmp(res->addr,tcp_pkt->dst_addr,16)==0)&&(res->port==tcp_pkt->dst_port);
    }else{
        return (req->ip==tcp_pkt->src_ip)&&(req->port==tcp_pkt->src_port)&&(res->ip == tcp_pkt->dst_ip)&&(res->port == tcp_pkt->dst_port);
    }

}

static inline int _endpoint_is_response(ch_tcp_session_endpoint_t *req,ch_tcp_session_endpoint_t *res,ch_packet_tcp_t *tcp_pkt){

    if(tcp_pkt->is_ipv6){
        return (req->is_ipv6)&&(memcmp(req->addr,tcp_pkt->dst_addr,16)==0)&&(req->port==tcp_pkt->dst_port)&&(res->is_ipv6)&&(memcmp(res->addr,tcp_pkt->src_addr,16)==0)&&(res->port==tcp_pkt->src_port);
    }else{
        return (req->ip==tcp_pkt->dst_ip)&&(req->port==tcp_pkt->dst_port)&&(res->ip == tcp_pkt->src_ip)&&(res->port == tcp_pkt->src_port);
    }

}

ch_tcp_session_endpoint_t * ch_tcp_session_endpoint_get(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){
   
    ch_tcp_session_endpoint_t *req = &tcp_session->endpoint_req;
    ch_tcp_session_endpoint_t *res = &tcp_session->endpoint_res;

    if(_endpoint_is_request(req,res,tcp_pkt)){
        /*request packet!*/
        return req;
    }

    if(_endpoint_is_response(req,res,tcp_pkt)){
        /*response packet!*/
        return res;
    }

    /*no belong this assemble session!*/
    return NULL;
}

ch_tcp_session_endpoint_t * ch_tcp_session_pair_endpoint_get(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    ch_tcp_session_endpoint_t *ep = NULL;
    ch_tcp_session_endpoint_t *req = &tcp_session->endpoint_req;
    ch_tcp_session_endpoint_t *res = &tcp_session->endpoint_res;

	ep = ch_tcp_session_endpoint_get(tcp_session,tcp_pkt);
	if(ep){
	
		return ep == req?res:req;
	}

	return NULL;
}
