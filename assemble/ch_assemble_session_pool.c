/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_pool.c
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
#include "ch_assemble_session_pool.h"

static uint32_t assemble_session_entry_hash(ch_four_tuple_t *tuple,void *priv_data){

    return ch_jhash_4words_sort(tuple->src_ip,tuple->dst_ip,
            (uint32_t)(tuple->src_port),(uint32_t)(tuple->dst_port),0);
}

static inline int _is_request(ch_assemble_session_t *ass,ch_four_tuple_t *tuple){

    int v1 = (ass->endpoint_req.ip == tuple->src_ip)&&(ass->endpoint_req.port == tuple->src_port);
    int v2 = (ass->endpoint_res.ip == tuple->dst_ip)&&(ass->endpoint_res.port == tuple->dst_port);
    
    return v1||v2;
}

static inline int _is_response(ch_assemble_session_t *ass,ch_four_tuple_t *tuple){

    int v1 = (ass->endpoint_req.ip == tuple->dst_ip)&&(ass->endpoint_req.port == tuple->dst_port);
    int v2 = (ass->endpoint_res.ip == tuple->src_ip)&&(ass->endpoint_res.port == tuple->src_port);
    
    return v1||v2;
}

static int assemble_session_entry_equal(ch_hash_entry_t *entry,ch_four_tuple_t *tuple,void *priv_data){

    ch_assemble_session_t *ass = (ch_assemble_session_t*)entry;

    return _is_request(ass,tuple)||_is_response(ass,tuple);
}

ch_assemble_session_pool_t * ch_assemble_session_pool_create(ch_context_t *context,ch_assemble_t *as){

    size_t n_sessions_limit = context->n_assemble_sessions_limit;
    apr_status_t rc;
    apr_pool_t *mp;

    /*create a memory pool for this assemble task*/
    rc = apr_pool_create(&mp,NULL);

    if(rc!=APR_SUCCESS||mp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create a memory pool for this assemble pool!");
        
        return NULL;
    }
    
    ch_assemble_session_pool_t *ass_pool = (ch_assemble_session_pool_t*)apr_palloc(mp,sizeof(ch_assemble_session_pool_t));

    ass_pool->mp = mp;

    ass_pool->hash_pool = ch_hash_pool_create(mp,sizeof(ch_assemble_session_t),n_sessions_limit,
            (void*)ass_pool,
            assemble_session_entry_hash,
            assemble_session_entry_equal);

    if(ass_pool->hash_pool == NULL){
        ch_log(CH_LOG_ERR,"Cannot create hash pool failed for assemble session pool!");
        return NULL;
    }

    ass_pool->n_sessions = 0;
    ass_pool->as = as;

    return ass_pool;
}


void ch_assemble_session_pool_destroy(ch_assemble_session_pool_t *ass_pool){

    if(ass_pool->mp){
        apr_pool_destroy(ass_pool->mp);
    }
}

ch_assemble_session_t * ch_assemble_session_pool_entry_create(ch_assemble_session_pool_t *ass_pool,ch_session_request_t *sreq,
        ch_four_tuple_t *tuple){

    ch_assemble_session_t *ass = (ch_assemble_session_t*)ch_hash_pool_entry_create(ass_pool->hash_pool,tuple);

    if(ass == NULL){
        ch_log(CH_LOG_ERR,"create assemble session failed!");
        return NULL;
    }

    if(ch_assemble_session_init(ass_pool->as,ass,sreq)){

        ch_log(CH_LOG_ERR,"init assemble session failed!");
        ch_assemble_session_pool_entry_free(ass_pool,ass);
        return NULL;
    }

    ass_pool->n_sessions +=1;
    return ass;
}

ch_assemble_session_t * ch_assemble_session_pool_entry_find(ch_assemble_session_pool_t *ass_pool,ch_four_tuple_t *tuple){

    return (ch_assemble_session_t*)ch_hash_pool_entry_find(ass_pool->hash_pool,tuple);

}

void ch_assemble_session_pool_entry_free(ch_assemble_session_pool_t *ass_pool,ch_assemble_session_t *ass){
    
    ch_assemble_session_fin(ass);

    ch_hash_pool_entry_free(ass_pool->hash_pool,(ch_hash_entry_t*)ass);

    ass_pool->n_sessions -=1;
}

