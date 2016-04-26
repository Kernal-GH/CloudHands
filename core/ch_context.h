/*
 * =====================================================================================
 *
 *       Filename:  ch_context.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时34分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_CONTEXT_H
#define CH_CONTEXT_H


typedef struct ch_context_t ch_context_t;

#include <apr_pools.h>
#include <apr_tables.h>
#include "ch_port_pool.h"
#include "ch_core_pool.h"
#include "ch_task_pool.h"
#include "ch_app_context.h"

#define LOG_NAME_DEFAULT "/tmp/cloudhands.log"
#define LOG_LEVEL_DEFAULT CH_LOG_NOTICE

#define MAX_PKT_BURST_DEFAULT 32
#define N_RX_MBUF_DEFAULT 8192
#define RTE_TEST_RX_DESC_DEFAULT 128
#define RTE_TEST_TX_DESC_DEFAULT 512
#define N_RX_QUEUE_DEFAULT 1
#define N_TX_QUEUE_DEFAULT 1

struct ch_context_t {

    apr_pool_t *mp;

    ch_app_context_t *app_context;

    ch_core_pool_t *cpool;

    ch_port_pool_t *ppool;

    ch_task_pool_t *tpool;

    /*config log name and level*/
    const char *log_name;
    int log_level;

    /*config core mask*/
    uint32_t core_mask;

    /*config port mask*/
    uint32_t port_mask;

    /*config rx port mask*/
    uint32_t port_rx_mask;

    /*config max pkt burst*/
    unsigned int port_max_pkt_burst;

    /*
     * Configurable number of RX/TX ring descriptors
     */
    unsigned int port_nb_rxd;
    unsigned int port_nb_txd;

    /*
     *Configurable number of RX/TX queue per port
     * */
    unsigned int port_n_rxq;
    unsigned int port_n_txq;

    /*Configurable number of RX mbuf */
    unsigned int n_rx_mbuf;

    /*Configurable size of assemble task's ring*/
    unsigned int astask_ring_size;

    /*Configurable number of assemble tasks*/
    unsigned int n_assemble_tasks;

    /*Configurable max number of session requests*/
    unsigned int n_session_requests_limit;

    /*Configurable max number of assemble task's sessions*/
    unsigned int n_assemble_sessions_limit;
};

extern ch_context_t * ch_context_create(apr_pool_t *mp,const char *cfname);

extern int ch_context_core_init(ch_context_t *context,uint32_t core_n);

extern int ch_context_port_init(ch_context_t *context,uint32_t port_n);

extern int ch_context_rxport_init(ch_context_t *context);

extern int ch_context_app_init(ch_context_t *context);

#endif /*CH_CONTEXT_H*/

