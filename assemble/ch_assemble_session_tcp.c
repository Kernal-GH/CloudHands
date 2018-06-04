/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_tcp.c
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

#include "ch_assemble_session_tcp.h"

int ch_assemble_session_tcp_init(ch_assemble_t *as,
        ch_assemble_session_tcp_t *ass,ch_tcp_session_request_t *sreq,ch_app_t *app){

    ch_assemble_session_endpoint_init(&ass->endpoint_req,sreq->req_ip,sreq->req_port,sreq->req_sn_init);

    ch_assemble_session_endpoint_init(&ass->endpoint_res,sreq->res_ip,sreq->res_port,sreq->res_sn_init);
   
    ass->state = SESSION_STATE_INIT; 

    ass->need_end = 0;
    ass->fin1_seq = 0;
    ass->fin2_seq = 0;
    ass->four_way_state = FOUR_WAY_INIT;

    ass->as = as;
    ass->app = NULL;
    ass->cur_ep = NULL;
    ass->app = app;

    return 0;
}

void ch_assemble_session_tcp_fin(ch_assemble_session_tcp_t *ass){

    ch_assemble_session_endpoint_fin(&ass->endpoint_req);
    ch_assemble_session_endpoint_fin(&ass->endpoint_res);
}

static inline int _endpoint_equal(ch_assemble_session_endpoint_t *ep,uint32_t ip,uint16_t port){

    return (ep->ip == ip)&&(ep->port == port);
}

ch_assemble_session_endpoint_t * ch_assemble_session_tcp_endpoint_get(ch_assemble_session_tcp_t *ass,ch_assemble_packet_t *as_pkt){
   
    ch_assemble_session_endpoint_t *req = &ass->endpoint_req;
    ch_assemble_session_endpoint_t *res = &ass->endpoint_res;

    if(_endpoint_equal(req,as_pkt->src_ip,as_pkt->src_port)&&_endpoint_equal(res,as_pkt->dst_ip,as_pkt->dst_port)){
        /*request packet!*/
        return req;
    }

    if(_endpoint_equal(req,as_pkt->dst_ip,as_pkt->dst_port)&&_endpoint_equal(res,as_pkt->src_ip,as_pkt->src_port)){
        /*response packet!*/
        return res;
    }

    /*no belong this assemble session!*/
    return NULL;
}

ch_assemble_session_endpoint_t * ch_assemble_session_tcp_pair_endpoint_get(ch_assemble_session_tcp_t *ass,ch_assemble_packet_t *as_pkt){

    ch_assemble_session_endpoint_t *ep = NULL;
    ch_assemble_session_endpoint_t *req = &ass->endpoint_req;
    ch_assemble_session_endpoint_t *res = &ass->endpoint_res;

	ep = ch_assemble_session_tcp_endpoint_get(ass,as_pkt);
	if(ep){
	
		return ep == req?res:req;
	}

	return NULL;
}
