/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_pool_tcp.c
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
#include "ch_assemble_session_pool_tcp.h"

static uint32_t assemble_session_entry_hash(void *key,void *priv_data){

	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)key;

    return ch_jhash_4words_sort(as_pkt->src_ip,as_pkt->dst_ip,
            (uint32_t)(as_pkt->src_port),(uint32_t)(as_pkt->dst_port),0);
}

static inline int _is_request(ch_assemble_session_tcp_t *ass,ch_assemble_packet_t *as_pkt){

    int v1 = (ass->endpoint_req.ip == as_pkt->src_ip)&&(ass->endpoint_req.port == as_pkt->src_port);
    int v2 = (ass->endpoint_res.ip == as_pkt->dst_ip)&&(ass->endpoint_res.port == as_pkt->dst_port);
    
    return v1&&v2;
}

static inline int _is_response(ch_assemble_session_tcp_t *ass,ch_assemble_packet_t *as_pkt){

    int v1 = (ass->endpoint_req.ip == as_pkt->dst_ip)&&(ass->endpoint_req.port == as_pkt->dst_port);
    int v2 = (ass->endpoint_res.ip == as_pkt->src_ip)&&(ass->endpoint_res.port == as_pkt->src_port);
    
    return v1&&v2;
}

static int assemble_session_entry_equal(ch_ptable_entry_t *entry,void *key,void *priv_data){

	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)key;

    ch_assemble_session_tcp_t *ass = (ch_assemble_session_tcp_t*)entry;

    return _is_request(ass,as_pkt)||_is_response(ass,as_pkt);
}

static void assemble_session_entry_clean(ch_ptable_entry_t *entry,void *priv_data){

	ch_assemble_session_pool_tcp_t *asspt = (ch_assemble_session_pool_tcp_t*)priv_data;

	ch_assemble_session_tcp_t *ass = (ch_assemble_session_tcp_t*)entry;

	ch_log(CH_LOG_WARN,"Will free a assemble tcp session,timeout or other ...!");
    
	ch_assemble_session_tcp_fin(ass);

    asspt->n_sessions -=1;
}

ch_assemble_session_pool_tcp_t * ch_assemble_session_pool_tcp_create(ch_assemble_t *as,ch_assemble_context_tcp_t *tcp_context){

    ch_assemble_session_pool_tcp_t *asspt;

    apr_status_t rc;
    apr_pool_t *mp;

    /*create a memory pool for this assemble task*/
    rc = apr_pool_create(&mp,NULL);

    if(rc!=APR_SUCCESS||mp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create a memory pool for this tcp assemble pool!");
        
        return NULL;
    }
    
    asspt = (ch_assemble_session_pool_tcp_t*)apr_palloc(mp,sizeof(ch_assemble_session_pool_tcp_t));

    asspt->mp = mp;

    asspt->as_tbl = ch_ptable_create(mp,0,sizeof(ch_assemble_session_tcp_t),
            tcp_context->tcp_assemble_sessions_limits,
            tcp_context->tcp_assemble_sessions_tbl_size,
            tcp_context->tcp_assemble_session_timeout,
            tcp_context->tcp_assemble_sessions_tbl_caches_limits,
            (void*)asspt,
            assemble_session_entry_hash,
            assemble_session_entry_equal,
            assemble_session_entry_clean);

    if(asspt->as_tbl == NULL){
        ch_log(CH_LOG_ERR,"Cannot create assemble table  failed for assemble tcp session pool!");
        return NULL;
    }

    rte_spinlock_init(&asspt->spin_lock);
    
	asspt->as = as;

    asspt->n_sessions = 0;

    return asspt;
}


void ch_assemble_session_pool_tcp_destroy(ch_assemble_session_pool_tcp_t *asspt){

    if(asspt->mp){
        apr_pool_destroy(asspt->mp);
    }
}

static inline void _assemble_session_free(ch_assemble_session_pool_tcp_t *asspt,ch_assemble_session_tcp_t *ass){

    ch_assemble_session_tcp_fin(ass);

    ch_ptable_entry_free(asspt->as_tbl,(ch_ptable_entry_t*)ass);

    asspt->n_sessions -=1;

}

ch_assemble_session_tcp_t * 
ch_assemble_session_pool_tcp_entry_create(ch_assemble_session_pool_tcp_t *asspt,ch_tcp_session_request_t *sreq,ch_app_t *app,
        ch_assemble_packet_t *as_pkt){

    ch_assemble_session_tcp_t *ass = NULL;

	ch_assemble_session_pool_lock(asspt);

	do{
		ass = (ch_assemble_session_tcp_t*)ch_ptable_entry_create(asspt->as_tbl,(void*)as_pkt);

		if(ass == NULL){
			ch_log(CH_LOG_ERR,"create tcp assemble session failed!");
			break;
		}

		if(ch_assemble_session_tcp_init(asspt->as,ass,sreq,app)){

			ch_log(CH_LOG_ERR,"init tcp assemble session failed!");
			_assemble_session_free(asspt,ass);
			ass = NULL;
			break;
		}
	}while(0);

	ch_assemble_session_pool_unlock(asspt);

    asspt->n_sessions +=1;
    return ass;
}

ch_assemble_session_tcp_t * ch_assemble_session_pool_tcp_entry_find(ch_assemble_session_pool_tcp_t *asspt,ch_assemble_packet_t *as_pkt){

	ch_assemble_session_tcp_t *ass = NULL;
	
	ch_assemble_session_pool_lock(asspt);

    ass = (ch_assemble_session_tcp_t*)ch_ptable_entry_find(asspt->as_tbl,(void*)as_pkt);

	ch_assemble_session_pool_unlock(asspt);

	return ass;
}

void ch_assemble_session_pool_tcp_entry_free(ch_assemble_session_pool_tcp_t *asspt,ch_assemble_session_tcp_t *ass){
    
	ch_assemble_session_pool_lock(asspt);
	_assemble_session_free(asspt,ass);
	ch_assemble_session_pool_unlock(asspt);
}

