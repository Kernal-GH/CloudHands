/*
 * =====================================================================================
 *
 *       Filename:  ch_port_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月25日 17时51分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_PORT_POOL_H
#define CH_PORT_POOL_H

typedef struct ch_port_pool_t ch_port_pool_t;
typedef struct ch_port_t ch_port_t;

#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <apr_tables.h>
#include "ch_context.h"

struct ch_port_pool_t {

    ch_context_t *context;
    apr_array_header_t *ports;

};

struct ch_port_t {
	struct rte_eth_dev_info dev_info;
    struct ether_addr   addr;

    struct rte_mbuf **rx_pkts_burst;
    unsigned int     n_rx_pkts;    

    ch_context_t    *context;
    unsigned int port_id;

    unsigned int max_pkt_burst;
    
    unsigned int nb_rxd;
    unsigned int nb_txd;

    unsigned int n_rxq;
    unsigned int n_txq;

};

extern ch_port_pool_t * ch_port_pool_create(ch_context_t *context);

extern size_t ch_port_pool_ports_bind(ch_port_pool_t *ppool,apr_array_header_t *arr,unsigned int port_mask);

extern int ch_port_pool_ports_setup(ch_port_pool_t *ppool,apr_array_header_t *arr,struct rte_mempool *pkt_pool,unsigned int socket);

#endif /*CH_PORT_POOL_H*/

