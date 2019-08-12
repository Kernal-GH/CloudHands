/*
 *
 *      Filename: ch_udp_session_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 13:32:56
 * Last Modified: 2018-09-13 17:00:06
 */

#include "ch_udp_session_pool.h"
#include "ch_udp_session.h"
#include "ch_jhash.h"
#include "ch_log.h"
 
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

static size_t _udp_session_entry_hash(void *key,void *priv_data ch_unused){

	ch_packet_udp_t *udp_pkt = (ch_packet_udp_t*)key;
    
    if(udp_pkt->is_ipv6){

        uint32_t addr_hash = ipv6_hash(udp_pkt->src_addr,udp_pkt->dst_addr);
        return ch_jhash_3words_sort(addr_hash,(uint32_t)(udp_pkt->src_port),(uint32_t)(udp_pkt->dst_port),0);
    }

    return (size_t)ch_jhash_4words_sort(udp_pkt->src_ip,udp_pkt->dst_ip,
            (uint32_t)(udp_pkt->src_port),(uint32_t)(udp_pkt->dst_port),0);
}

static inline int _is_request(ch_udp_session_t *udp_session,ch_packet_udp_t *udp_pkt){

    int v1 = (udp_session->endpoint_req.ip == udp_pkt->src_ip)&&(udp_session->endpoint_req.port == udp_pkt->src_port);
    int v2 = (udp_session->endpoint_res.ip == udp_pkt->dst_ip)&&(udp_session->endpoint_res.port == udp_pkt->dst_port);
    
    return v1&&v2;
}

static inline int _is_response(ch_udp_session_t *udp_session,ch_packet_udp_t *udp_pkt){

    int v1 = (udp_session->endpoint_req.ip == udp_pkt->dst_ip)&&(udp_session->endpoint_req.port == udp_pkt->dst_port);
    int v2 = (udp_session->endpoint_res.ip == udp_pkt->src_ip)&&(udp_session->endpoint_res.port == udp_pkt->src_port);
    
    return v1&&v2;
}

static int _udp_session_entry_equal(ch_ptable_entry_t *entry,void *key,void *priv_data ch_unused){

	ch_packet_udp_t *udp_pkt = (ch_packet_udp_t*)key;

    ch_udp_session_t *udp_session = (ch_udp_session_t*)entry;

    return is_udp_request(udp_session,udp_pkt)||is_udp_response(udp_session,udp_pkt);
}

ch_udp_session_pool_t *ch_udp_session_pool_create(ch_udp_work_t *udp_work,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data){

    ch_udp_session_pool_t *udp_pool;
	ch_udp_context_t *udp_context = udp_work->udp_context;

    ch_pool_t *mp;

    /*create a memory pool for this assemble task*/
    mp = ch_pool_create(4096);

    if(mp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create a memory pool for this udp session pool!");
        return NULL;
    }
    
    udp_pool = (ch_udp_session_pool_t*)ch_palloc(mp,sizeof(ch_udp_session_pool_t));

    udp_pool->mp = mp;
	udp_pool->work = udp_work;

    udp_pool->udp_session_tbl = ch_ptable_create(mp,
		    udp_context->udp_session_pool_type, 
		    sizeof(ch_udp_session_t),
			0,
            udp_context->udp_session_limits,
            udp_context->udp_session_tbl_size,
            udp_context->udp_session_timeout,
            udp_context->udp_session_cache_limits,
            priv_data,
            _udp_session_entry_hash,
            _udp_session_entry_equal,
            NULL,
			entry_timeout_cb);

    if(udp_pool->udp_session_tbl == NULL){
        ch_log(CH_LOG_ERR,"Cannot create  udp session table  failed for udp udp session pool!");
        return NULL;
    }
    

    return udp_pool;

	
}

ch_udp_session_t *
ch_udp_session_pool_entry_create(ch_udp_session_pool_t *udp_pool,ch_packet_udp_t *udp_pkt,ch_udp_app_session_t *app_session,
	uint64_t session_id){


	ch_udp_session_t *udp_session = (ch_udp_session_t*)ch_ptable_entry_create(udp_pool->udp_session_tbl,(void*)udp_pkt);
	if(udp_session)
	{

		ch_udp_session_init(udp_session,udp_pkt,app_session,session_id);

	}

	return udp_session;

}

