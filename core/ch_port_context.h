/*
 *
 *      Filename: ch_port_context.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-08-23 00:46:13
 * Last Modified: 2016-08-23 00:46:13
 */

#ifndef CH_PORT_CONTEXT_H
#define CH_PORT_CONTEXT_H

typedef struct ch_port_context_t ch_port_context_t;

#include "ch_context.h"

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

struct ch_port_context_t {

	ch_context_t *context;

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

extern ch_port_context_t* ch_port_context_create(ch_context_t *context);

#endif /* CH_PORT_CONTEXT_H */
