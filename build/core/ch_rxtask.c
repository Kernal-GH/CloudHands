/*
 * =====================================================================================
 *
 *       Filename:  ch_rxtask.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月16日 11时18分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <rte_cycles.h>
#include "ch_constants.h"
#include "ch_rxtask.h"
#include "ch_log.h"

#define PREFETCH_OFFSET 3



/*this function will be called before receive the packet by task's core*/

static int rx_task_init(ch_task_t *task,void *priv_data){


    return CH_OK;
}

static void _pkt_process(ch_rxtask_t *rxtsk,struct rte_mbuf *mbuf,uint64_t time){

    unsigned int j,n;
    int ret;

    ch_rxtask_processor_t *processor;
    ch_rxtask_processor_t **processors = (ch_rxtask_processor_t**)rxtsk->context->rxtp->pkt_processors->elts;
    n = rxtsk->context->rxtp->pkt_processors->nelts;

	if(rxtsk->context->debug_rxpacket == 0){
		/*call mbuf processors*/
		for(j = 0; j<n; j++){
			processor = processors[j];
			if(processor->process){
				ret = processor->process(mbuf,time,processor->priv_data);

				if(ret == PROCESSOR_RET_DROP){

					rte_pktmbuf_free(mbuf);
					break;
				}
			}
		}
	}else{
	
		rte_pktmbuf_free(mbuf);
	}

}


static unsigned int read_pkt_from_port_queue(ch_rxtask_t *rxtsk,ch_port_queue_t *pq,uint64_t time){
    
    unsigned int packets = 0;
    int j,nb_rx;

    struct rte_mbuf **pkts_burst = pq->rx_pkts_burst;
    struct rte_mbuf *mbuf;
	ch_port_t *port = pq->port;

	nb_rx = rte_eth_rx_burst(port->port_id,pq->pq_id,pkts_burst,port->pcontext->port_max_pkt_burst);
	
	if(nb_rx == 0){
		return 0;
	}

	packets+=nb_rx;

	/* Prefetch first packets */
	for (j = 0; j < PREFETCH_OFFSET && j < nb_rx; j++) {
		rte_prefetch0(rte_pktmbuf_mtod(
				pkts_burst[j], void *));
	}

	/* Prefetch and process already prefetched packets */
	for (j = 0; j < (nb_rx - PREFETCH_OFFSET); j++) {
		rte_prefetch0(rte_pktmbuf_mtod(pkts_burst[
				j + PREFETCH_OFFSET], void *));
	   
		_pkt_process(rxtsk,pkts_burst[j],time);
	}
	
	/* Process remaining prefetched packets */
	for (; j < nb_rx; j++) {
		_pkt_process(rxtsk,pkts_burst[j],time);
	}

    return packets;
}

/*this function will be called to receive the packet from rx ports by task's core*/
static int rx_task_run(ch_task_t *task,void *priv_data){
    
    int i;
    uint64_t time;
    unsigned int packets,p=0;

    ch_rxtask_t *rxtask = (ch_rxtask_t*)task;


    /*Read packet from RX queues*/
    time = 0;//rte_rdtsc();


	packets = read_pkt_from_port_queue(rxtask,rxtask->pq,time);

	rxtask->rx_packets += packets;

    return TASK_RETURN_OK;
} 

/*this function will be called to release resources when exit by task's core*/

static int rx_task_fin(ch_task_t *tk,void *priv_data){

    return CH_OK;
}

/*create a rx task,the rxtask will assign to a core,
 * and some ports will be specified rx ports by rx port mask*/

ch_task_t * ch_rxtask_create(ch_context_t *context,unsigned int id){

    ch_core_t *core;
	ch_port_queue_t *pq;

    ch_rxtask_t *rx_task = (ch_rxtask_t*)apr_palloc(context->mp,sizeof(ch_rxtask_t));

    rx_task->context = context;
    rx_task->task.init = rx_task_init;
    rx_task->task.run = rx_task_run;
    rx_task->task.exit = rx_task_fin;
    rx_task->task.tsk_id = id;

	rx_task->rx_packets = 0;

    /*bind to cpu core*/
    if(CH_ERROR == ch_core_pool_task_bind(context->cpool,(ch_task_t*)rx_task)){
        ch_log(CH_LOG_ERR,"The recieve task cannot be bound to a cpu core!");
        return NULL;
    }
    core = rx_task->task.core;

	rx_task->pq = ch_port_queue_bind(context->port,core->socket);
	if(rx_task->pq == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot bind any port queue for this rcv task!");
        ch_core_pool_task_unbind(context->cpool,core);
		return NULL;
	}

    return (ch_task_t*)rx_task;
}

