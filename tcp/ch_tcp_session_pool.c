/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月14日 13时13分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_jhash.h"
#include "ch_log.h"
#include "ch_tcp_session_pool.h"

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

#define ipv6_addr_equal(a,b) (memcmp(a,b,16)==0)

static size_t tcp_session_entry_hash(void *key,void *priv_data ch_unused){

	ch_packet_tcp_t *tcp_pkt = (ch_packet_tcp_t*)key;

    if(tcp_pkt->is_ipv6){

        uint32_t addr_hash = ipv6_hash(tcp_pkt->src_addr,tcp_pkt->dst_addr);
        return ch_jhash_3words_sort(addr_hash,(uint32_t)(tcp_pkt->src_port),(uint32_t)(tcp_pkt->dst_port),0);
    }

    return (size_t)ch_jhash_4words_sort(tcp_pkt->src_ip,tcp_pkt->dst_ip,
            (uint32_t)(tcp_pkt->src_port),(uint32_t)(tcp_pkt->dst_port),0);
}

static inline int _is_request(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    int v1,v2;

    if(tcp_pkt->is_ipv6){
        v1 = (tcp_session->endpoint_req.is_ipv6)&&(ipv6_addr_equal(tcp_session->endpoint_req.addr,tcp_pkt->src_addr))&&(tcp_session->endpoint_req.port == tcp_pkt->src_port);
        v2 = (tcp_session->endpoint_res.is_ipv6)&&(ipv6_addr_equal(tcp_session->endpoint_res.addr,tcp_pkt->dst_addr))&&(tcp_session->endpoint_res.port == tcp_pkt->dst_port);
    }else{
        v1 = (tcp_session->endpoint_req.ip == tcp_pkt->src_ip)&&(tcp_session->endpoint_req.port == tcp_pkt->src_port);
        v2 = (tcp_session->endpoint_res.ip == tcp_pkt->dst_ip)&&(tcp_session->endpoint_res.port == tcp_pkt->dst_port);
    }

    return v1&&v2;
}

static inline int _is_response(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    int v1,v2;

    if(tcp_pkt->is_ipv6){
        v1 = (tcp_session->endpoint_req.is_ipv6)&&(ipv6_addr_equal(tcp_session->endpoint_req.addr,tcp_pkt->dst_addr))&&(tcp_session->endpoint_req.port == tcp_pkt->dst_port);
        v2 = (tcp_session->endpoint_res.is_ipv6)&&(ipv6_addr_equal(tcp_session->endpoint_res.addr,tcp_pkt->src_addr))&&(tcp_session->endpoint_res.port == tcp_pkt->src_port);

    }else{
        v1 = (tcp_session->endpoint_req.ip == tcp_pkt->dst_ip)&&(tcp_session->endpoint_req.port == tcp_pkt->dst_port);
        v2 = (tcp_session->endpoint_res.ip == tcp_pkt->src_ip)&&(tcp_session->endpoint_res.port == tcp_pkt->src_port);
    }

    return v1&&v2;
}

static int tcp_session_entry_equal(ch_ptable_entry_t *entry,void *key,void *priv_data ch_unused){

	ch_packet_tcp_t *tcp_pkt = (ch_packet_tcp_t*)key;

    ch_tcp_session_t *tcp_session = (ch_tcp_session_t*)entry;

    return _is_request(tcp_session,tcp_pkt)||_is_response(tcp_session,tcp_pkt);
}

static void tcp_session_entry_clean(ch_ptable_entry_t *entry,void *priv_data ch_unused){

	ch_tcp_session_t *tcp_session = (ch_tcp_session_t*)entry;

	ch_log(CH_LOG_WARN,"Will free a assemble tcp session,timeout or other ...!");
    
	ch_tcp_session_fin(tcp_session);

}

ch_tcp_session_pool_t * ch_tcp_session_pool_create(ch_tcp_context_t *tcp_context,size_t priv_data_size,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data){

    ch_tcp_session_pool_t *tspt;

    ch_pool_t *mp;

    /*create a memory pool for this assemble task*/
    mp = ch_pool_create(4096);

    if(mp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create a memory pool for this tcp session pool!");
        return NULL;
    }
    
    tspt = (ch_tcp_session_pool_t*)ch_palloc(mp,sizeof(ch_tcp_session_pool_t));

	if(priv_data == NULL)
		priv_data = (void*)tspt;

    tspt->mp = mp;
	tspt->tcp_context = tcp_context;

    tspt->tcp_session_tbl = ch_ptable_create(mp,
		    tcp_context->tcp_session_pool_type, 
		    sizeof(ch_tcp_session_t),
			priv_data_size,
            tcp_context->tcp_session_limits,
            tcp_context->tcp_session_tbl_size,
            tcp_context->tcp_session_timeout,
            tcp_context->tcp_session_cache_limits,
            priv_data,
            tcp_session_entry_hash,
            tcp_session_entry_equal,
            tcp_session_entry_clean,
			entry_timeout_cb);

    if(tspt->tcp_session_tbl == NULL){
        ch_log(CH_LOG_ERR,"Cannot create tcp session table  failed for tcp session pool!");
        return NULL;
    }
    

	tspt->mm = ch_memory_create(mp,tcp_context->mm_max_cache_size,tcp_context->mm_timeout);
	if(tspt->mm == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create memory for tcp session pool!");
	
		return NULL;
	}

    return tspt;
}


void ch_tcp_session_pool_destroy(ch_tcp_session_pool_t *tspt){

    if(tspt->mp){
        ch_pool_destroy(tspt->mp);
    }
}

ch_tcp_session_t * 
ch_tcp_session_pool_entry_create(ch_tcp_session_pool_t *tspt,ch_tcp_session_request_t *sreq,ch_tcp_app_t *app,
	    void *sentry,
        ch_packet_tcp_t *tcp_pkt){

    ch_tcp_session_t *tcp_session = NULL;

	do{
       
        tcp_session = (ch_tcp_session_t*)ch_ptable_entry_create(tspt->tcp_session_tbl,(void*)tcp_pkt);

		if(tcp_session == NULL){
			ch_log(CH_LOG_ERR,"create tcp session failed!");
			break;
		}
		
		tcp_session->ts_pool =tspt;

		if(ch_tcp_session_init(tcp_session,sreq,app,sentry,tspt->mm)){

			ch_log(CH_LOG_ERR,"init tcp  session failed!");
			ch_tcp_session_pool_entry_free(tspt,tcp_session);
			tcp_session = NULL;
			break;
		}
	}while(0);


    return tcp_session;
}

ch_tcp_session_t * ch_tcp_session_pool_entry_find(ch_tcp_session_pool_t *tspt,ch_packet_tcp_t *tcp_pkt){

	ch_tcp_session_t *tcp_session = NULL;
	

    tcp_session = (ch_tcp_session_t*)ch_ptable_entry_find(tspt->tcp_session_tbl,(void*)tcp_pkt);


	return tcp_session;
}

