/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月21日 10时06分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 */

#include "ch_assemble_session.h"

int ch_assemble_session_init(ch_assemble_task_t *astask,ch_assemble_session_t *ass,ch_session_request_t *sreq){

    ch_assemble_session_endpoint_init(&ass->endpoint_req,sreq->req_ip,sreq->req_port,sreq->req_sn_init);

    ch_assemble_session_endpoint_init(&ass->endpoint_res,sreq->res_ip,sreq->res_port,sreq->res_sn_init);
   
    ass->state = SESSION_STATE_INIT; 

    ass->need_end = 0;
    ass->fin1_seq = 0;
    ass->fin2_seq = 0;
    ass->four_way_state = FOUR_WAY_INIT;

    ass->astask = astask;
    ass->app = NULL;
    ass->cur_ep = NULL;

    return 0;
}

void ch_assemble_session_fin(ch_assemble_session_t *ass){

    ch_assemble_session_endpoint_fin(&ass->endpoint_req);
    ch_assemble_session_endpoint_fin(&ass->endpoint_res);
}

static inline int _endpoint_equal(ch_assemble_session_endpoint_t *ep,uint32_t ip,uint16_t port){

    return (ep->ip == ip)&&(ep->port == port);
}

ch_assemble_session_endpoint_t * ch_assemble_session_endpoint_get(ch_assemble_session_t *ass,ch_four_tuple_t *tuple){
   
    ch_assemble_session_endpoint_t *req = &ass->endpoint_req;
    ch_assemble_session_endpoint_t *res = &ass->endpoint_res;

    if(_endpoint_equal(req,tuple->src_ip,tuple->src_port)&&_endpoint_equal(res,tuple->dst_ip,tuple->dst_port)){
        /*request packet!*/
        return req;
    }

    if(_endpoint_equal(req,tuple->dst_ip,tuple->dst_port)&&_endpoint_equal(res,tuple->src_ip,tuple->src_port)){
        /*response packet!*/
        return res;
    }

    /*no belong this assemble session!*/
    return NULL;
}

