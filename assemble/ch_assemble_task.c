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
#include "ch_constants.h"
#include "ch_log.h"
#include "ch_assemble_task.h"
#include "ch_assemble.h"
#include "ch_task.h"

#define RING_SIZE_N_DEFAULT 100000



static int assemble_task_init(ch_task_t *task,void *data){

    return CH_OK;
}

static inline int _is_should_flush(ch_assemble_task_t *astask){

	return (astask->context->debug_mode)&&(astask->context->debug_mode_flush_count<astask->no_packets_count);
}

static int assemble_task_run(ch_task_t *task,void *data){

    ch_assemble_task_t *astask = (ch_assemble_task_t*)task;
    struct rte_mbuf *mbuf;

    if(rte_ring_dequeue(astask->pkts,(void **)(&mbuf))){
        /* no packet */
		
		astask->no_packets_count +=1;
		if(_is_should_flush(astask)){
		
			ch_mmap_file_format_flush(astask->as->mmfmt);
			astask->no_packets_count = 0;

		}

		//return TASK_RETURN_OK;
        return TASK_RETURN_SLEEP;
    }

	astask->no_packets_count = 0;

    ch_assemble_do(astask->as,mbuf);

    return TASK_RETURN_OK;
} 

static int assemble_task_exit(ch_task_t *task,void *data){

    return CH_OK;
} 

ch_task_t * ch_assemble_task_create(ch_context_t *context,ch_assemble_t *as,const char *prefix,unsigned int task_id){

    ch_core_t *core;
    ch_assemble_task_t *astask;
    int i;
    char ring_name[64]={0};
    astask = (ch_assemble_task_t*)apr_palloc(context->mp,sizeof(ch_assemble_task_t));

    astask->context = context;
    astask->task.init = assemble_task_init;
    astask->task.run = assemble_task_run;
    astask->task.exit = assemble_task_exit;
    astask->task.tsk_id = task_id;
    
    astask->stats.packets = 0;
    astask->stats.sessions = 0;
    astask->stats.last_time = 0;
	astask->no_packets_count = 0;

    astask->task_id = task_id;
    astask->ring_size = context->astask_ring_size == 0?RING_SIZE_N_DEFAULT: context->astask_ring_size;


    /*bind to cpu core*/
    if(CH_ERROR == ch_core_pool_task_bind(context->cpool,(ch_task_t*)astask)){
        ch_log(CH_LOG_ERR,"The assemble task[%u] can not be bound to a cpu core!",task_id);
        return NULL;
    }

    core = astask->task.core;

    astask->as = as;

    /*create ring to accept packets from low level protocol!*/
    snprintf(ring_name,63,"%s_%d",prefix,task_id);
    astask->pkts = rte_ring_create(ring_name,astask->ring_size,core->socket,RING_F_SC_DEQ);

    if(astask->pkts == NULL){
        /*failed*/
        ch_log(CH_LOG_ERR,"Cannot create rte_ring[name:%s,size:%d] to store packets for assemble task!",
                ring_name,astask->ring_size);

        ch_assemble_fin(astask->as);
        ch_core_pool_task_unbind(context->cpool,core);
        return NULL;
    }

    /*ok*/
    as->as_task = astask;
    return (ch_task_t*)astask;
}



int ch_assemble_task_pkt_put(ch_assemble_task_t *astask,ch_assemble_packet_t *as_pkt){
 

    /*if ring queue is full,then wait to comsume some packets*/
    if(rte_ring_full(astask->pkts)){

        ch_log(CH_LOG_ERR," The rxtask[%d] ring is full[%d],will drop this packet------------------",
                astask->task_id,rte_ring_count(astask->pkts));
        
        return -1;
    }
    
    if(rte_ring_enqueue(astask->pkts,(void*)as_pkt->mbuf)!=0){
        
        /*Enqueue failed!*/
        ch_log(CH_LOG_ERR,"enqueue assemble task's queue failed!");

        return -1;
    }

    return 0;
}


