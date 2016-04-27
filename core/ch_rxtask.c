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
#define CH_RX_TASK_MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)
#define CH_RX_TASK_MBUF_CACHE_SIZE 256

/*this function will be called before receive the packet by task's core*/

static int rx_task_init(ch_task_t *task,void *priv_data){

    ch_rxtask_t *rxtsk = (ch_rxtask_t*)task;
    ch_context_t* context = rxtsk->context;

    /*setup all recieve ports*/
    if(CH_ERROR == ch_port_pool_ports_setup(context->ppool,rxtsk->rx_ports,rxtsk->pktmbuf_pool,task->core->socket)){

        ch_log(CH_LOG_ERR,"Cannot setup ports to receive packets!");
        return CH_ERROR;
    }

    return CH_OK;
}

static void _pkt_process(ch_rxtask_t *rxtsk,ch_port_t *port,struct rte_mbuf *mbuf,uint64_t time){

    unsigned int j,n;
    int ret;

    ch_rxtask_processor_t *processor;
    ch_rxtask_processor_t **processors = (ch_rxtask_processor_t**)rxtsk->pkt_processors->elts;
    n = rxtsk->pkt_processors->nelts;

    /*call mbuf processors*/
    for(j = 0; j<n; j++){
        processor = processors[j];
        if(processor->process){
            ret = processor->process(port,mbuf,time,processor->priv_data);

            if(ret == PROCESSOR_RET_DROP){

                ch_log(CH_LOG_INFO,"will drop a pkt by processor[%s],discard process it!",processor->name);
                rte_pktmbuf_free(mbuf);
                break;
            }
        }
    }
}

static unsigned int read_pkt_from_port(ch_rxtask_t *rxtsk,ch_port_t *port,uint64_t time){
    
    unsigned int packets = 0;
    unsigned int nb_rx,i;
    int ret;
    int j;

    struct rte_mbuf **pkts_burst = port->rx_pkts_burst;
    struct rte_mbuf *mbuf;
    
    for(i=0;i<port->n_rxq;i++){

        nb_rx = rte_eth_rx_burst(port->port_id,i,pkts_burst,port->max_pkt_burst);

        if(nb_rx == 0){
            continue;
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
            
            _pkt_process(rxtsk,port,pkts_burst[j],time);
        }
        
        /* Process remaining prefetched packets */
        for (; j < nb_rx; j++) {
            _pkt_process(rxtsk,port,pkts_burst[j],time);
        }
    }

    port->n_rx_pkts = packets;
    return packets;
}

/*this function will be called to receive the packet from rx ports by task's core*/

static int rx_task_run(ch_task_t *task,void *priv_data){
    
    int i;
    uint64_t time;
    unsigned int packets = 0,p=0;

    ch_port_t *port;
    ch_rxtask_t *rxtask = (ch_rxtask_t*)task;
    ch_port_t **ports = (ch_port_t**)(rxtask->rx_ports->elts);

    /*Read packet from RX queues*/
    time = rte_rdtsc();

    for(i=0; i<rxtask->rx_ports->nelts;i++){

        port = ports[i];

        p = read_pkt_from_port(rxtask,port,time);

        packets+=p;
    }

    if(packets == 0){
        return TASK_RETURN_SLEEP; 
    }

    return TASK_RETURN_OK;
} 

/*this function will be called to release resources when exit by task's core*/

static int rx_task_fin(ch_task_t *tk,void *priv_data){

    return CH_OK;
}

/*create a rx task,the rxtask will assign to a core,
 * and some ports will be specified rx ports by rx port mask*/

ch_task_t * ch_rxtask_create(ch_context_t *context){

    ch_core_t *core;
    size_t n_ports;

    ch_rxtask_t *rx_task = (ch_rxtask_t*)apr_palloc(context->mp,sizeof(ch_rxtask_t));

    rx_task->context = context;
    rx_task->task.init = rx_task_init;
    rx_task->task.run = rx_task_run;
    rx_task->task.exit = rx_task_fin;
    rx_task->task.tsk_id = 0;

    rx_task->rx_ports = apr_array_make(context->mp,16,sizeof(ch_port_t *));

    rx_task->pkt_processors = apr_array_make(context->mp,16,sizeof(ch_rxtask_processor_t*));

    /*bind recieve ports*/
    n_ports = ch_port_pool_ports_bind(context->ppool,rx_task->rx_ports,context->port_rx_mask);
    if(n_ports == 0){
        ch_log(CH_LOG_ERR,"No ethernet port used to bind recieve task!");
        return NULL;
    }else{
        ch_log(CH_LOG_INFO,"The recieve task was bound to ports[%lu]",n_ports);
    }

    /*bind to cpu core*/
    if(CH_ERROR == ch_core_pool_task_bind(context->cpool,(ch_task_t*)rx_task)){
        ch_log(CH_LOG_ERR,"The recieve task cannot be bound to a cpu core!");
        return NULL;
    }
    core = rx_task->task.core;

    /*alloc packet pool */
    rx_task->pktmbuf_pool = rte_mempool_create("CH_RXTASK_BUF",
            context->n_rx_mbuf,
            CH_RX_TASK_MBUF_SIZE,
            CH_RX_TASK_MBUF_CACHE_SIZE, 
			sizeof(struct rte_pktmbuf_pool_private),
			rte_pktmbuf_pool_init, NULL, rte_pktmbuf_init, NULL,
            core->socket,0
            );

    if(rx_task->pktmbuf_pool == NULL){
        ch_log(CH_LOG_ERR,"create packet mbuf pool failed!");
        ch_core_pool_task_unbind(context->cpool,core);
        return NULL;
    }

    rx_task->task.core = core;
    
    /*create ethertype*/
    rx_task->ethertype = ch_ethertype_create(context,rx_task,&rx_task->pinfo);

    if(rx_task->ethertype == NULL){
        ch_log(CH_LOG_ERR,"create ethertype failed !");
        ch_core_pool_task_unbind(context->cpool,core);
        return NULL;
    }

    /*create ipv4 */
    rx_task->ipv4 = ch_ipv4_create(context,rx_task->ethertype);
    if(rx_task->ipv4 == NULL){
        ch_log(CH_LOG_ERR,"create ipv4 failed!");
        ch_core_pool_task_unbind(context->cpool,core);
        return NULL;
    }

    /*create tcp*/
    rx_task->tcp = ch_tcp_create(context,rx_task->ipv4);
    if(rx_task->tcp == NULL){
        ch_log(CH_LOG_ERR,"create tcp failed!");
        ch_core_pool_task_unbind(context->cpool,core);
        return NULL;
    }
    
    return (ch_task_t*)rx_task;
}

void ch_rxtask_pkt_processor_register(ch_rxtask_t *task,ch_rxtask_processor_t *prosessor){

    *(ch_rxtask_processor_t**)apr_array_push(task->pkt_processors) = prosessor;
}
