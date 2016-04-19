/*
 * =====================================================================================
 *
 *       Filename:  ch_port.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时50分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_PORT_H
#define CH_PORT_H

typedef struct ch_port_t ch_port_t;

#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include "ch_context.h"

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

extern ch_port_t * ch_port_create(ch_context_t *context,unsigned int port_id);

extern int ch_port_setup(ch_port_t *port,struct rte_mempool *pkt_pool,unsigned int socket);
#endif /*CH_PORT_H*/
