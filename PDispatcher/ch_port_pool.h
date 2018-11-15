/*
 * =====================================================================================
 *
 *       Filename:  ch_port_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2017 09:35:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_PORT_POOL_H
#define CH_PORT_POOL_H

typedef struct ch_port_pool_t ch_port_pool_t;
typedef struct ch_port_t ch_port_t;
typedef struct ch_port_queue_t ch_port_queue_t;
typedef struct ch_port_context_t ch_port_context_t;

#include <rte_cycles.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

#include "ch_list.h"
#include "ch_mpool.h"
#include "ch_tables.h"

struct ch_port_pool_t {

   ch_pool_t *mp;

   ch_port_context_t *pcontext;

   ch_array_header_t *ports;
   
   struct rte_mempool *pktmbuf_pool;

   //struct rte_mempool *sambuf_pool;

   int port_n;
   int cur_port_index;
   int port_queue_n;

   int port_queue_c;
};

struct ch_port_t {
   
   struct rte_eth_dev_info dev_info;
   struct ether_addr   addr;
   struct rte_eth_conf port_conf;
   struct rte_eth_rxconf rx_conf;		/**< RX configuration */
   struct rte_eth_link link;			/**< Link Information like speed and duplex */
   
   ch_port_pool_t *ppool;
   
   ch_array_header_t *port_queues;

   int port_id;
   
   int port_queue_n;
   int cur_port_queue_index;
};

struct ch_port_queue_t {

   struct list_head node;


   ch_port_t *port;

   int queue_id;

};

struct ch_port_context_t {
   
   int8_t rx_pthresh;
   int8_t rx_hthresh;
   int8_t rx_wthresh;

  /*
   * Configurable value of RX free threshold.
   */
  int16_t rx_free_thresh;

  /*
   * Configurable value of RX drop enable.
   */
  int8_t rx_drop_en;

  /*
   * Receive Side Scaling (RSS) configuration.
   */
  uint64_t rss_hf;

  /*config max pkt burst*/
  uint16_t port_max_pkt_burst;
   
  /*
   * Configurable number of RX/TX ring descriptors
   */
  uint16_t port_nb_rxd;
  
   /*
   *Configurable number of RX/TX queue per port
  * */
  uint16_t port_n_rxq;

  /*Configurable number of RX mbuf */
  uint32_t n_rx_mbuf;
  
  /*Configurable size of rx mbuf*/
  uint32_t rx_mbuf_size;
  
  /*Configurable cache size of rx mbuf*/
  uint32_t rx_mbuf_cache_size;

  uint32_t jumbo_frame_max_size;
  
  int jumbo_frame;

};

#define MAX_PKT_BURST_DEFAULT 32
#define N_RX_MBUF_DEFAULT 8192
#define RTE_TEST_RX_DESC_DEFAULT 128
#define N_RX_QUEUE_DEFAULT 1

#define RTE_PMD_PARAM_UNSET -1

enum {
	RX_PTHRESH              = 8,	/**< Default values of RX prefetch threshold reg. */
	RX_HTHRESH              = 8,	/**< Default values of RX host threshold reg. */
	RX_WTHRESH              = 4,	/**< Default values of RX write-back threshold reg. */
};

/* RSS HF STRING */
#define RSS_HF_IP  "rss_ip"
#define RSS_HF_TCP "rss_tcp"
#define RSS_HF_UDP "rss_udp"
#define RSS_HF_SCTP "rss_sctp"

extern ch_port_pool_t * ch_port_pool_create(ch_pool_t *mp,const char *cfname,const char *pkt_pool_name,const char *sa_pool_name,uint32_t port_mask);

extern ch_port_context_t* ch_port_context_create(ch_pool_t *mp,const char *cfname);

extern int ch_port_pool_setup(ch_port_pool_t *ppool);

extern ch_port_queue_t * ch_port_pool_queue_bind(ch_port_pool_t *ppool,uint32_t socket);

extern void ch_port_pool_stat_dump(ch_port_t *port,FILE *fp);

#endif /*CH_PORT_POOL_H*/
