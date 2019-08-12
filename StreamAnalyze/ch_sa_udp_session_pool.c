/*
 *
 *      Filename: ch_sa_udp_session_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 13:32:56
 * Last Modified: 2018-04-13 15:29:30
 */

#include "ch_sa_udp_session_pool.h"
#include "ch_sa_udp_session.h"
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

static size_t _sa_udp_session_entry_hash(void *key,void *priv_data ch_unused){

	ch_packet_udp_t *udp_pkt = (ch_packet_udp_t*)key;

    if(udp_pkt->is_ipv6){

        uint32_t addr_hash = ipv6_hash(udp_pkt->src_addr,udp_pkt->dst_addr);
        return ch_jhash_3words_sort(addr_hash,(uint32_t)(udp_pkt->src_port),(uint32_t)(udp_pkt->dst_port),0);
    }
    return (size_t)ch_jhash_4words_sort(udp_pkt->src_ip,udp_pkt->dst_ip,
            (uint32_t)(udp_pkt->src_port),(uint32_t)(udp_pkt->dst_port),0);
}


static inline int _is_request(ch_sa_udp_session_t *sa_udp_session,ch_packet_udp_t *sa_udp_pkt){

    int v1 = (sa_udp_session->endpoint_req.ip == sa_udp_pkt->src_ip)&&(sa_udp_session->endpoint_req.port == sa_udp_pkt->src_port);
    int v2 = (sa_udp_session->endpoint_res.ip == sa_udp_pkt->dst_ip)&&(sa_udp_session->endpoint_res.port == sa_udp_pkt->dst_port);
    
    return v1&&v2;
}

static inline int _is_response(ch_sa_udp_session_t *sa_udp_session,ch_packet_udp_t *sa_udp_pkt){

    int v1 = (sa_udp_session->endpoint_req.ip == sa_udp_pkt->dst_ip)&&(sa_udp_session->endpoint_req.port == sa_udp_pkt->dst_port);
    int v2 = (sa_udp_session->endpoint_res.ip == sa_udp_pkt->src_ip)&&(sa_udp_session->endpoint_res.port == sa_udp_pkt->src_port);
    
    return v1&&v2;
}

static int _sa_udp_session_entry_equal(ch_ptable_entry_t *entry,void *key,void *priv_data ch_unused){

	ch_packet_udp_t *sa_udp_pkt = (ch_packet_udp_t*)key;

    ch_sa_udp_session_t *sa_udp_session = (ch_sa_udp_session_t*)entry;

    return is_udp_request(sa_udp_session,sa_udp_pkt)||is_udp_response(sa_udp_session,sa_udp_pkt);
}

ch_sa_udp_session_pool_t *ch_sa_udp_session_pool_create(ch_sa_work_t *sa_work,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data){

    ch_sa_udp_session_pool_t *udp_pool;
	ch_sa_context_t *sa_context = sa_work->sa_context;

    ch_pool_t *mp;

    /*create a memory pool for this assemble task*/
    mp = ch_pool_create(4096);

    if(mp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create a memory pool for this sa_udp session pool!");
        return NULL;
    }
    
    udp_pool = (ch_sa_udp_session_pool_t*)ch_palloc(mp,sizeof(ch_sa_udp_session_pool_t));

    udp_pool->mp = mp;
	udp_pool->sa_work = sa_work;

    udp_pool->udp_session_tbl = ch_ptable_create(mp,
		    sa_context->udp_session_pool_type, 
		    sizeof(ch_sa_udp_session_t),
			0,
            sa_context->udp_session_limits,
            sa_context->udp_session_tbl_size,
            sa_context->udp_session_timeout,
            sa_context->udp_session_cache_limits,
            priv_data,
            _sa_udp_session_entry_hash,
            _sa_udp_session_entry_equal,
            NULL,
			entry_timeout_cb);

    if(udp_pool->udp_session_tbl == NULL){
        ch_log(CH_LOG_ERR,"Cannot create sa udp session table  failed for sa udp session pool!");
        return NULL;
    }
    

    return udp_pool;

	
}
