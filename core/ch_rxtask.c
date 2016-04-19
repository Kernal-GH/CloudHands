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

#define CH_RX_TASK_MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)
#define CH_RX_TASK_MBUF_CACHE_SIZE 256

/*this function will be called before receive the packet by task's core*/

static int rx_task_init(ch_task_t *task,void *user_data){

    return CH_OK;
}

static void read_pkt_from_port(ch_rxtask_t *task,ch_port_t *port,uint64_t time){
    
    unsigned int nb_rx,i;
    int ret;
    int j;

    struct rte_mbuf **pkts_burst = port->rx_pkts_burst;
    struct rte_mbuf *mbuf;
    
    ch_rxtask_processor_t *processor;
    ch_rxtask_processor_t **processors = (ch_rxtask_processor_t**)task->pkt_processors->elts;

    for(i=0;i<port->n_rxq;i++){

        nb_rx = rte_eth_rx_burst(port->port_id,i,pkts_burst,port->max_pkt_burst);

        if(nb_rx == 0){
            return;
        }

        //ch_log(CH_LOG_INFO,"Receive pkts[%d] from port[%d]",nb_rx,port->port_id);
        port->n_rx_pkts = nb_rx;

        for(i=0;i<nb_rx;i++){

            mbuf = pkts_burst[i];

            /*call mbuf processors*/
            for(j = 0; j<task->pkt_processors->nelts; j++){

                processor = processors[j];
                if(processor->process){
                    ret = processor->process(port,mbuf,time,processor->user_data);

                    if(ret == PROCESSOR_RET_DROP){

                        ch_log(CH_LOG_INFO,"will drop a pkt by processor[%s],discard process it!",processor->name);
                        break;
                    }
                }
            }
           
            /*mbuf has been processed by all processors,free it*/
            rte_pktmbuf_free(mbuf);
        }
    }
}

/*this function will be called to receive the packet from rx ports by task's core*/

static int rx_task_run(ch_task_t *task,void *user_data){
    
    int i;
    uint64_t time;

    ch_port_t *port;
    ch_rxtask_t *rxtask = (ch_rxtask_t*)task;
    ch_port_t **ports = (ch_port_t**)(rxtask->rx_ports->elts);

    /*if no rx ports,return*/
    if(rxtask->rx_ports->nelts == 0){
        ch_log(CH_LOG_ERR,"no port to recieve packets");
        return CH_ERROR;
    }

    while(1){

        /*Read packet from RX queues*/

        time = rte_rdtsc();

        for(i=0; i<rxtask->rx_ports->nelts;i++){

            port = ports[i];

            read_pkt_from_port(rxtask,port,time);
        }
    }

    return CH_OK;
} 

/*this function will be called to release resources when exit by task's core*/

static int rx_task_fin(ch_task_t *tk,void *user_data){

    return CH_OK;
}

/*create a rx task,the rxtask will assign to a core,
 * and some ports will be specified rx ports by rx port mask*/

ch_task_t * ch_rxtask_create(ch_context_t *context,unsigned int rx_port_mask){

    int i;

    ch_core_t *core;
    ch_port_t *port;
    ch_port_t **ports;

    /*must specify rx port mask*/
    if(rx_port_mask == 0){
        ch_log(CH_LOG_ERR,"Must specify a valid rx port mask!");
        return NULL;
    }
    
    ch_rxtask_t *rx_task = (ch_rxtask_t*)apr_palloc(context->mp,sizeof(ch_rxtask_t));

    rx_task->task.init = rx_task_init;
    rx_task->task.run = rx_task_run;
    rx_task->task.exit = rx_task_fin;

    rx_task->rx_ports = apr_array_make(context->mp,16,sizeof(ch_port_t *));

    rx_task->pkt_processors = apr_array_make(context->mp,16,sizeof(ch_rxtask_processor_t*));

    /*select recieve ports*/
    ports = (ch_port_t **)context->ports->elts;
    for(i = 0;i<context->ports->nelts;i++){

        port = ports[i];

        if((rx_port_mask&(1<<port->port_id))==0)
            continue;
        ch_log(CH_LOG_DEBUG,"select port[%d] for recieving packts",port->port_id);
        *(ch_port_t**)apr_array_push(rx_task->rx_ports) = port;
    }

    if(rx_task->rx_ports->nelts == 0){

        ch_log(CH_LOG_ERR,"no port specified to recieve packets!");
        return NULL;
    }

    /*get a free core from context*/
    core = ch_context_core_pop(context);
    if(core == NULL){
        ch_log(CH_LOG_ERR,"Cannot get a core to run rx task!");
        return NULL;
    }
    
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
        ch_context_core_push(context,core);

        return NULL;
    }


    /*setup all rx ports*/
    ports = (ch_port_t**)rx_task->rx_ports->elts;

    for(i=0;i<rx_task->rx_ports->nelts;i++){
        port = ports[i];

        if(ch_port_setup(port,rx_task->pktmbuf_pool,core->socket)!=CH_OK){
            ch_log(CH_LOG_ERR,"cannot setup rx port:%d",port->port_id);
                
            ch_context_core_push(context,core);
            
            return NULL;
        }
    }

    rx_task->task.core = core;
    
    /*assign the task to core*/
    ch_core_task_assign(core,(ch_task_t*)rx_task);

    /*create ethertype*/
    rx_task->ethertype = ch_ethertype_create(context,rx_task,&rx_task->pinfo);

    if(rx_task->ethertype == NULL){
        ch_log(CH_LOG_ERR,"create ethertype failed !");
        return NULL;
    }

    /*create ipv4 */
    rx_task->ipv4 = ch_ipv4_create(context,rx_task->ethertype);
    if(rx_task->ipv4 == NULL){
        ch_log(CH_LOG_ERR,"create ipv4 failed!");
        return NULL;
    }

    /*create tcp*/
    rx_task->tcp = ch_tcp_create(context,rx_task->ipv4);
    if(rx_task->tcp == NULL){
        ch_log(CH_LOG_ERR,"create tcp failed!");
        return NULL;
    }
    
    return (ch_task_t*)rx_task;
}

void ch_rxtask_pkt_processor_register(ch_rxtask_t *task,ch_rxtask_processor_t *prosessor){

    *(ch_rxtask_processor_t**)apr_array_push(task->pkt_processors) = prosessor;
}
