/*
 * =====================================================================================
 *
 *       Filename:  ch_port.h
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

#ifndef CH_PORT_H
#define CH_PORT_H

typedef struct ch_port_t ch_port_t;
typedef struct ch_port_queue_t ch_port_queue_t;

#include <rte_cycles.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>
#include <apr_tables.h>
#include "ch_context.h"
#include "ch_port_context.h"

struct ch_port_t {
	struct rte_eth_dev_info dev_info;
    struct ether_addr   addr;
	struct rte_eth_conf port_conf;
	struct rte_eth_rxconf rx_conf;		/**< RX configuration */
	struct rte_eth_link link;			/**< Link Information like speed and duplex */

	ch_port_context_t *pcontext;

	ch_context_t *context;
	apr_array_header_t *queues;
    

	int  port_id;

	int pq_index;
};

struct ch_port_queue_t {

	struct rte_mempool *pktmbuf_pool;
	struct rte_mbuf **rx_pkts_burst;
	ch_port_t *port;

	int pq_id;

	unsigned int socket;
};

extern ch_port_t * ch_port_create(ch_context_t *context,unsigned int port_mask);

extern void ch_port_destroy(ch_port_t *port);

extern ch_port_queue_t * ch_port_queue_bind(ch_port_t *port,unsigned int socket);

extern int ch_port_setup(ch_port_t *port);

#endif /*CH_PORT_H*/

