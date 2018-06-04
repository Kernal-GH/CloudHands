/*
 *
 *      Filename: ch_rxtask_pool.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-08-03 01:53:42
 * Last Modified: 2017-01-19 11:49:42
 */
#include <rte_mempool.h>
#include "ch_log.h"
#include "ch_rxtask_pool.h"
#include "ch_util.h"
#include "ch_task.h"
#include "ch_rxtask.h"

uint64_t ch_rxtask_pool_packets_get(ch_rxtask_pool_t *rxtp){

	uint64_t packets = 0;
	unsigned int i,n = rxtp->rxtasks->nelts;
	ch_rxtask_t *rxt ;
	ch_rxtask_t ** arr = (ch_rxtask_t**)rxtp->rxtasks->elts;

	for(i = 0; i<n;i++ ){
	
		rxt = arr[i];
		packets+= rxt->rx_packets;

		printf("rx:%u,packets:%lu\n",rxt->task.tsk_id,(unsigned long)rxt->rx_packets);

	}

	return packets;
}

void ch_rxtask_pool_dump(ch_rxtask_pool_t *rxtp){

	uint64_t packets = 0;
	unsigned int i,n = rxtp->rxtasks->nelts;
	ch_rxtask_t *rxt ;
	ch_rxtask_t ** arr = (ch_rxtask_t**)rxtp->rxtasks->elts;

	printf("Dump rxtask pool info:\n");

	for(i = 0; i<n;i++ ){
	
		rxt = arr[i];
		packets+= rxt->rx_packets;

		printf("rx:%u,packets:%lu\n",rxt->task.tsk_id,(unsigned long)rxt->rx_packets);

	}
	
	printf("rcv packets:%lu\n",(unsigned long)packets);

	for(i = 0; i<n;i++ ){
	
		rxt = arr[i];

		printf("Dump rx:%u pkt mem pool info:\n",rxt->task.tsk_id);

		rte_mempool_dump(stdout,rxt->pq->pktmbuf_pool);
		
	}

}

static inline void _rxtask_pool_task_put(ch_rxtask_pool_t *rxtp,ch_task_t *tk){

    apr_array_header_t *arr = rxtp->rxtasks;

    *(ch_task_t**)apr_array_push(arr) = tk; 
}

ch_rxtask_pool_t * ch_rxtask_pool_create(ch_context_t *context){


	unsigned int i;
	unsigned int n_rxtasks = CH_MIN(context->n_rxtasks,context->port->pcontext->port_n_rxq);
	ch_task_t *tsk;

	if(n_rxtasks == 0){
	
		ch_log(CH_LOG_ERR,"rcv task number is zero,nothing to do!");
		return NULL;
	}

	ch_rxtask_pool_t *rxtp = NULL;


	rxtp = (ch_rxtask_pool_t*)apr_palloc(context->mp,sizeof(*rxtp));

	rxtp->context = context;
	rxtp->rxtasks = apr_array_make(context->mp,16,sizeof(ch_rxtask_t*));
	rxtp->pkt_processors = apr_array_make(context->mp,16,sizeof(ch_rxtask_processor_t*));

    /*create ethertype*/
    rxtp->ethertype = ch_ethertype_create(context,rxtp);

    if(rxtp->ethertype == NULL){
        ch_log(CH_LOG_ERR,"create ethertype failed !");
        return NULL;
    }

    /*create ipv4 */
    rxtp->ipv4 = ch_ipv4_create(context,rxtp->ethertype);
    if(rxtp->ipv4 == NULL){
        ch_log(CH_LOG_ERR,"create ipv4 failed!");
        return NULL;
    }

    /*create tcp*/
    rxtp->tcp = ch_tcp_create(context,rxtp->ipv4);
    if(rxtp->tcp == NULL){
        ch_log(CH_LOG_ERR,"create tcp failed!");
        return NULL;
    }
   
    /*create udp*/
    rxtp->udp = ch_udp_create(context,rxtp->ipv4);
    if(rxtp->udp == NULL){
        ch_log(CH_LOG_ERR,"create udp failed!");
        return NULL;
    }

	/*create all rxtasks */
	for(i = 0; i<n_rxtasks;i++){
	
		tsk = ch_rxtask_create(context,i+1);
		if(tsk == NULL){
		
			ch_log(CH_LOG_ERR,"Create rcv packet task:%u failed!",i+1);
			return NULL;
		}

		_rxtask_pool_task_put(rxtp,tsk);
	}

	return rxtp;
}
