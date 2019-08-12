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

 static inline uint32_t
 ipv6_hash(uint8_t *src_addr,uint8_t *dst_addr)
 {
     uint32_t *word_src_addr = (uint32_t *)&(src_addr[0]);
     uint32_t *word_dst_addr = (uint32_t *)&(dst_addr[0]);
 
     return (word_src_addr[0] ^ word_dst_addr[0]) ^
             (word_src_addr[1] ^ word_dst_addr[1]) ^
             (word_src_addr[2] ^ word_dst_addr[2]) ^
             (word_src_addr[3] ^ word_dst_addr[3]);
 }

static size_t session_request_entry_hash(void *key,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	ch_packet_tcp_t *tcp_pkt = (ch_packet_tcp_t*)key;

    if(tcp_pkt->is_ipv6){
        uint32_t addr_hash = ipv6_hash(tcp_pkt->src_addr,tcp_pkt->dst_addr);
        return (size_t)ch_jhash_3words_sort(addr_hash,(uint32_t)(tcp_pkt->src_port),(uint32_t)(tcp_pkt->dst_port),0);
    }

    return (size_t)ch_jhash_4words_sort(tcp_pkt->src_ip,tcp_pkt->dst_ip,
            (uint32_t)(tcp_pkt->src_port),(uint32_t)(tcp_pkt->dst_port),0);
}

static inline int _is_request(ch_tcp_session_request_t *r,ch_packet_tcp_t *tcp_pkt){

    if(tcp_pkt->is_ipv6){

        return (r->is_ipv6)&&(memcmp(r->src_addr,tcp_pkt->src_addr,16)==0)&&(r->req_port==tcp_pkt->src_port)&&(memcmp(r->dst_addr,tcp_pkt->dst_addr,16)==0)&&(r->res_port==tcp_pkt->dst_port);
    }

    return (r->req_ip == tcp_pkt->src_ip&&r->req_port == tcp_pkt->src_port)&&(r->res_ip == tcp_pkt->dst_ip&&r->res_port == tcp_pkt->dst_port);
}

static inline int _is_response(ch_tcp_session_request_t *r,ch_packet_tcp_t *tcp_pkt){

    if(tcp_pkt->is_ipv6)
    {
        return (r->is_ipv6)&&(memcmp(r->src_addr,tcp_pkt->dst_addr,16)==0)&&(r->req_port==tcp_pkt->dst_port)&&(memcmp(r->dst_addr,tcp_pkt->src_addr,16)==0)&&(r->res_port==tcp_pkt->src_port);

    }

    return (r->req_ip == tcp_pkt->dst_ip&&r->req_port == tcp_pkt->dst_port)&&(r->res_ip == tcp_pkt->src_ip&&r->res_port == tcp_pkt->src_port);
}

static int session_request_entry_equal(ch_ptable_entry_t *entry,void *key,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	ch_packet_tcp_t *tcp_pkt = (ch_packet_tcp_t*)key;

    ch_tcp_session_request_t *request = (ch_tcp_session_request_t*)entry;
    
    return _is_request(request,tcp_pkt)||_is_response(request,tcp_pkt);
}

static void session_request_entry_clean(ch_ptable_entry_t *entry,void *priv_data){

	/*unused*/
	entry = entry;
	priv_data = priv_data;
}

ch_tcp_session_request_pool_t * ch_tcp_session_request_pool_create(ch_tcp_context_t *tcp_context,size_t priv_data_size,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data){

    ch_tcp_session_request_pool_t *req_pool;
	
	ch_pool_t *mp = ch_pool_create(4096);
	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create pool for tcp session request pool!");
		return NULL;
	}

    req_pool = (ch_tcp_session_request_pool_t*)ch_palloc(mp,sizeof(ch_tcp_session_request_pool_t));
    
	if(priv_data == NULL)
		priv_data = (void*)req_pool;

    req_pool->tcp_session_request_tbl = ch_ptable_create(mp,
		    tcp_context->tcp_session_request_pool_type,
		    sizeof(ch_tcp_session_request_t),
			priv_data_size,
            tcp_context->tcp_session_request_limits,
            tcp_context->tcp_session_request_tbl_size,
            tcp_context->tcp_session_request_timeout,
            tcp_context->tcp_session_request_cache_limits,
            priv_data,
            session_request_entry_hash,
            session_request_entry_equal,
            session_request_entry_clean,
			entry_timeout_cb);

    if(req_pool->tcp_session_request_tbl == NULL){
        ch_log(CH_LOG_ERR,"Cannot create tcp session request table for tcp session request pool!");
        ch_pool_destroy(mp);
        return NULL;
    }

    req_pool->tcp_context = tcp_context;
    req_pool->mp = mp;
    req_pool->n_requests = 0;

    return req_pool;
}

void ch_tcp_session_request_pool_destroy(ch_tcp_session_request_pool_t *req_pool){

    if(req_pool->mp){
        
        ch_pool_destroy(req_pool->mp);
    }

}

ch_tcp_session_request_t* 
ch_tcp_session_request_create(ch_tcp_session_request_pool_t *req_pool,ch_packet_tcp_t *tcp_pkt){
   
    ch_tcp_session_request_t *req = (ch_tcp_session_request_t*)ch_ptable_entry_create(req_pool->tcp_session_request_tbl,(void*)tcp_pkt);

    if(req == NULL){

        ch_log(CH_LOG_ERR,"Create tcp session request failed!");
        return NULL;
    }
    req->is_ipv6 = tcp_pkt->is_ipv6;
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

    ch_ptable_entry_free(req_pool->tcp_session_request_tbl,(ch_ptable_entry_t*)req);
    req_pool->n_requests -=1;
}

ch_tcp_session_request_t* ch_tcp_session_request_find(ch_tcp_session_request_pool_t *req_pool,ch_packet_tcp_t *tcp_pkt){

    return (ch_tcp_session_request_t*)ch_ptable_entry_find(req_pool->tcp_session_request_tbl,(void*)tcp_pkt);
}

