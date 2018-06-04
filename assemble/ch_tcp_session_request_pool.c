/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session_request_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月29日 11时42分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_log.h"
#include "ch_jhash.h"
#include "ch_util.h"
#include "ch_tcp_session_request_pool.h"

static uint32_t session_request_entry_hash(void *key,void *priv_data){

	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)key;

    return ch_jhash_4words_sort(as_pkt->src_ip,as_pkt->dst_ip,
            (uint32_t)(as_pkt->src_port),(uint32_t)(as_pkt->dst_port),0);
}

static inline int _is_request(ch_tcp_session_request_t *r,ch_assemble_packet_t *as_pkt){

    return (r->req_ip == as_pkt->src_ip&&r->req_port == as_pkt->src_port)&&(r->res_ip == as_pkt->dst_ip&&r->res_port == as_pkt->dst_port);
}

static inline int _is_response(ch_tcp_session_request_t *r,ch_assemble_packet_t *as_pkt){

    return (r->req_ip == as_pkt->dst_ip&&r->req_port == as_pkt->dst_port)&&(r->res_ip == as_pkt->src_ip&&r->res_port == as_pkt->src_port);
}

static int session_request_entry_equal(ch_ptable_entry_t *entry,void *key,void *priv_data){

	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)key;

    ch_tcp_session_request_t *request = (ch_tcp_session_request_t*)entry;
    
    return _is_request(request,as_pkt)||_is_response(request,as_pkt);
}

static void session_request_entry_clean(ch_ptable_entry_t *entry,void *priv_data){

}

ch_tcp_session_request_pool_t * ch_tcp_session_request_pool_create(ch_assemble_context_tcp_t *tcp_context){

    int i;
    apr_pool_t *mp;
    ch_tcp_session_request_pool_t *req_pool;

    apr_pool_create(&mp,NULL);

    if(mp == NULL){
        ch_log(CH_LOG_ERR,"Create memory pool failed for tcp session request pool!");
        return NULL;
    }

    req_pool = (ch_tcp_session_request_pool_t*)apr_palloc(mp,sizeof(ch_tcp_session_request_pool_t));
    
    req_pool->as_tbl = ch_ptable_create(mp,0,sizeof(ch_tcp_session_request_t),
            tcp_context->tcp_session_requests_limits,
            tcp_context->tcp_session_requests_tbl_size,
            tcp_context->tcp_session_request_timeout,
            tcp_context->tcp_session_requests_tbl_caches_limits,
            (void*)req_pool,
            session_request_entry_hash,
            session_request_entry_equal,
            session_request_entry_clean);

    if(req_pool->as_tbl == NULL){
        ch_log(CH_LOG_ERR,"Cannot create assemble table for tcp session request pool!");
        apr_pool_destroy(mp);
        return NULL;
    }

    req_pool->tcp_context = tcp_context;
    req_pool->mp = mp;
    req_pool->n_requests = 0;

    return req_pool;
}

void ch_tcp_session_request_pool_destroy(ch_tcp_session_request_pool_t *req_pool){

    if(req_pool->mp){
        
        apr_pool_destroy(req_pool->mp);
    }
}

ch_tcp_session_request_t* 
ch_tcp_session_request_create(ch_tcp_session_request_pool_t *req_pool,ch_assemble_packet_t *as_pkt){
   
    ch_tcp_session_request_t *req = (ch_tcp_session_request_t*)ch_ptable_entry_create(req_pool->as_tbl,(void*)as_pkt);

    if(req == NULL){

        ch_log(CH_LOG_ERR,"Create tcp session request failed!");
        return NULL;
    }

    req->req_sn_init = 0;
    req->res_sn_init = 0;
    req->req_ip = 0;
    req->req_port = 0;
    req->res_ip = 0;
    req->res_port = 0;
    req->three_way_state = THREE_WAY_INIT;
    
    req_pool->n_requests +=1;
 
    return req;
}

void ch_tcp_session_request_free(ch_tcp_session_request_pool_t* req_pool,ch_tcp_session_request_t *req){

    ch_ptable_entry_free(req_pool->as_tbl,(ch_ptable_entry_t*)req);
    req_pool->n_requests -=1;
}

ch_tcp_session_request_t* ch_tcp_session_request_find(ch_tcp_session_request_pool_t *req_pool,ch_assemble_packet_t *as_pkt){

    return (ch_tcp_session_request_t*)ch_ptable_entry_find(req_pool->as_tbl,(void*)as_pkt);
}

