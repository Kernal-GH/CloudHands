/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_worker.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月16日 18时02分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_util.h"
#include "ch_constants.h"
#include "ch_jhash.h"
#include "ch_log.h"
#include "ch_assemble_task.h"
#include "ch_assemble.h"

#define RING_SIZE_N_DEFAULT 100000

static int assemble_task_init(ch_task_t *task,void *data){

    return CH_OK;
}

static int assemble_task_run(ch_task_t *task,void *data){

    ch_assemble_task_t *astask = (ch_assemble_task_t*)task;
    struct rte_mbuf *mbuf;

    while(1){

        if(rte_ring_dequeue(astask->pkts,(void **)(&mbuf))){
            /* no packet */
            usleep(5);
            continue;
        }

        ch_assemble_do(astask,mbuf);
    }

    return CH_OK;
} 

static int assemble_task_exit(ch_task_t *task,void *data){

    return CH_OK;
} 

ch_task_t * ch_assemble_task_create(ch_context_t *context,uint16_t task_id){

    ch_assemble_task_t *astask;
    ch_core_t *cpu_core;
    apr_pool_t *mp;
    int i;
    apr_status_t rc;
    char ring_name[64]={0};

    /*get a cpu core from context instance*/
    cpu_core = ch_context_core_pop(context);
    if(cpu_core == NULL){
        ch_log(CH_LOG_ERR,"Cannot get a cpu core to run assemble task!");
        return NULL;
    }
    
    /*create a memory pool for this assemble task*/
    rc = apr_pool_create(&mp,NULL);

    if(rc!=APR_SUCCESS||mp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create a memory pool for this assemble task:%d\n",task_id);
        
        /*push cpu core back into context*/
        ch_context_core_push(context,cpu_core);

        return NULL;
    }

    astask = (ch_assemble_task_t*)apr_palloc(context->mp,sizeof(ch_assemble_task_t));
    if(astask == NULL){
        ch_log(CH_LOG_ERR,"No memory to create the instance of assemble task!");
        apr_pool_destroy(mp);
        /*push cpu core back into context*/
        ch_context_core_push(context,cpu_core);
        return NULL;
    }

    astask->ass_pool = ch_assemble_session_pool_create(astask,mp,context->n_assemble_sessions_limit);
    if(astask->ass_pool == NULL){
        ch_log(CH_LOG_ERR,"Create assemble session's pool failed for this assemble task!");
        apr_pool_destroy(mp);
        /*push cpu core back into context*/
        ch_context_core_push(context,cpu_core);
        return NULL;
        
    }
    astask->context = context;
    astask->mp = context->mp;
    astask->task.init = assemble_task_init;
    astask->task.run = assemble_task_run;
    astask->task.exit = assemble_task_exit;


    astask->task_id = task_id;
    astask->ring_size = context->astask_ring_size == 0?RING_SIZE_N_DEFAULT: context->astask_ring_size;

    /*create ring to accept packets from low level protocol!*/
    snprintf(ring_name,63,"astask_%d",task_id);
    astask->pkts = rte_ring_create(ring_name,astask->ring_size,cpu_core->socket,RING_F_SP_ENQ|RING_F_SC_DEQ);
    if(astask->pkts == NULL){
        /*failed*/
        ch_log(CH_LOG_ERR,"Cannot create rte_ring[name:%s,size:%d] to store packets for assemble task!",
                ring_name,astask->ring_size);
        
        apr_pool_destroy(mp);

        ch_context_core_push(context,cpu_core);
        return NULL;
    }

    astask->task.core = cpu_core;
    /*bind this assemble task into specify cpu core*/
    ch_core_task_assign(cpu_core,(ch_task_t*)astask);


    /*ok*/

    return (ch_task_t*)astask;
}

ch_assemble_session_t * ch_assemble_task_session_find(ch_assemble_task_t *astask,ch_packet_info_t *pinfo){

    
    ch_four_tuple_t tuple;
    ch_four_tuple_init(&tuple,pinfo->mbuf);

    return ch_assemble_session_pool_entry_find(astask->ass_pool,&tuple);
}

ch_assemble_session_t *ch_assemble_task_session_create(ch_assemble_task_t *astask,ch_session_request_t *sreq,ch_packet_info_t *pinfo){

    ch_four_tuple_t tuple;

    ch_four_tuple_init(&tuple,pinfo->mbuf);
    
    return ch_assemble_session_pool_entry_create(astask->ass_pool,sreq,&tuple);
}

int ch_assemble_task_pkt_put(ch_assemble_task_t *astask,ch_packet_info_t *pinfo){
  
    /*if ring queue is full,then wait to comsume some packets*/
    while(rte_ring_full(astask->pkts) == 1){

        ch_log(CH_LOG_DEBUG,"waiting the assemble task[%d] to comsume packets[%d]------------------",
                astask->task_id,rte_ring_count(astask->pkts));
        
        usleep(5);
    }
    
    if(rte_ring_enqueue(astask->pkts,(void*)pinfo->mbuf)!=0){
        
        /*Enqueue failed!*/
        ch_log(CH_LOG_ERR,"enqueue assemble task's queue failed!");

        return -1;
    }

    return 0;
}
