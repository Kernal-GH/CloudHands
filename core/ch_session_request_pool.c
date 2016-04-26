/*
 * =====================================================================================
 *
 *       Filename:  ch_session_request_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月05日 17时00分12秒
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
#include "ch_session_request_pool.h"

static uint32_t session_request_entry_hash(ch_four_tuple_t *tuple,void *priv_data){

    return ch_jhash_4words_sort(tuple->src_ip,tuple->dst_ip,
            (uint32_t)(tuple->src_port),(uint32_t)(tuple->dst_port),0);
}

static inline int _is_request(ch_session_request_t *r,ch_four_tuple_t *tuple){

    return (r->req_ip == tuple->src_ip&&r->req_port == tuple->src_port)&&(r->res_ip == tuple->dst_ip&&r->res_port == tuple->dst_port);
}

static inline int _is_response(ch_session_request_t *r,ch_four_tuple_t *tuple){

    return (r->req_ip == tuple->dst_ip&&r->req_port == tuple->dst_port)&&(r->res_ip == tuple->src_ip&&r->res_port == tuple->src_port);
}

static int session_request_entry_equal(ch_hash_entry_t *entry,ch_four_tuple_t *tuple,void *priv_data){


    ch_session_request_t *request = (ch_session_request_t*)entry;
    
    return _is_request(request,tuple)||_is_response(request,tuple);
}

ch_session_request_pool_t * ch_session_request_pool_create(ch_context_t *context){

    int i;
    apr_pool_t *mp;
    ch_session_request_pool_t *req_pool;
    ch_task_t *astask;

    apr_pool_create(&mp,NULL);

    if(mp == NULL){
        ch_log(CH_LOG_ERR,"Create memory pool failed for session request pool!");
        return NULL;
    }

    req_pool = (ch_session_request_pool_t*)apr_palloc(mp,sizeof(ch_session_request_pool_t));
    
    req_pool->hash_pool = ch_hash_pool_create(mp,sizeof(ch_session_request_t),
            context->n_session_requests_limit,(void*)req_pool,
            session_request_entry_hash,
            session_request_entry_equal
            );

    if(req_pool->hash_pool == NULL){
        ch_log(CH_LOG_ERR,"Cannot create hash pool for session request pool!");
        apr_pool_destroy(mp);
        return NULL;
    }

    req_pool->context = context;
    req_pool->mp = mp;
    req_pool->n_requests = 0;

    return req_pool;
}

void ch_session_request_pool_destroy(ch_session_request_pool_t *req_pool){

    if(req_pool->mp){
        
        apr_pool_destroy(req_pool->mp);
    }
}

ch_session_request_t* ch_session_request_create(ch_session_request_pool_t *req_pool,ch_packet_info_t *pinfo){
   
    ch_four_tuple_t tuple;

    ch_four_tuple_init3(&tuple,pinfo->src_ip,pinfo->dst_ip,pinfo->src_port,pinfo->dst_port);

    ch_session_request_t *req = (ch_session_request_t*)ch_hash_pool_entry_create(req_pool->hash_pool,&tuple);

    if(req == NULL){

        ch_log(CH_LOG_ERR,"Create session request failed!");
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

void ch_session_request_free(ch_session_request_pool_t* req_pool,ch_session_request_t *req){

    ch_hash_pool_entry_free(req_pool->hash_pool,(ch_hash_entry_t*)req);
    req_pool->n_requests -=1;
}

ch_session_request_t* ch_session_request_find(ch_session_request_pool_t *req_pool,ch_packet_info_t *pinfo){

    ch_four_tuple_t tuple;

    ch_four_tuple_init3(&tuple,pinfo->src_ip,pinfo->dst_ip,pinfo->src_port,pinfo->dst_port);
    
    return (ch_session_request_t*)ch_hash_pool_entry_find(req_pool->hash_pool,&tuple);
}

static inline int _need_update_assemble_task(ch_assemble_task_t *astask,size_t n_packets,size_t n_sessions){

    size_t c_ring = (size_t)rte_ring_count(astask->pkts);
    size_t cur_sessions_n = ch_assemble_task_n_sessions_get(astask);

    return (cur_sessions_n <n_sessions)||(cur_sessions_n == n_sessions&&c_ring<n_packets);

}

ch_assemble_task_t * ch_assemble_task_get_from_request_pool(ch_session_request_pool_t *req_pool){

    int i;
    size_t n_packets = 0,n_sessions = 0;

    ch_assemble_task_t *astask = NULL,*res = NULL,**astasks;

    astasks = (ch_assemble_task_t **)req_pool->assemble_tasks->elts;

    for(i = 0; i< req_pool->assemble_tasks->nelts; i++){

        astask = astasks[i];
        if(res == NULL){
            res = astask;

        }else{

            if(_need_update_assemble_task(astask,n_packets,n_sessions)){
                
                res = astask;
                n_packets = (size_t)rte_ring_count(astask->pkts);
                n_sessions = ch_assemble_task_n_sessions_get(astask);
            }
        }
    }

    return res;
}
