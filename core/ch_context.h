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

#include "ch_task.h"
#include "ch_port.h"
#include "ch_core_pool.h"

#include "ch_rxtask_pool.h"
#include "ch_app_context.h"
#include "ch_packets_interface.h"
#include "ch_black_white_list.h"

#define LOG_NAME_DEFAULT "/tmp/cloudhands.log"
#define LOG_LEVEL_DEFAULT CH_LOG_NOTICE

#define MAX_PKT_BURST_DEFAULT 32
#define N_RX_MBUF_DEFAULT 8192
#define RTE_TEST_RX_DESC_DEFAULT 128
#define RTE_TEST_TX_DESC_DEFAULT 512
#define N_RX_QUEUE_DEFAULT 1
#define N_TX_QUEUE_DEFAULT 1

#define MMAP_FILE_DIR_DEFAULT   "/tmp"
#define MMAP_FILE_SIZE_DEFAULT  (4*1024*1024*1024L)
#define MMAP_FILE_ENTRY_SIZE_DEFAULT (16*1024*1024L)

struct ch_context_t {

    apr_pool_t *mp;

    ch_core_pool_t *cpool;

    ch_port_t *port;

	ch_rxtask_pool_t *rxtp;

	ch_packets_interface_t *pin;

	ch_black_white_list_t *bwl;

    /*config log name and level*/
    const char *log_name;
    int log_level;

    /*config core mask*/
    uint32_t core_mask;

    /*config rx port mask*/
    uint32_t port_rx_mask;

	unsigned int n_rxtasks;

    /*Configurable size of assemble task's ring*/
    unsigned int astask_ring_size;

    /*configurable assemble tcp context file path*/
    const char *tcp_cfname; 

	/* configurable assemble udp context file path */
	const char *udp_cfname;

    /*configurable tcp app context file path*/
    const char *app_tcp_cfname;

    /*configurable udp app context file path*/
    const char *app_udp_cfname;
	
	/**/
	const char *port_cfname;

	const char *packets_cfname;

	const char *crawl_cfname;

	int debug_mode;
	size_t debug_mode_flush_count;

	int debug_rxpacket;
};

extern ch_context_t * ch_context_create(apr_pool_t *mp,const char *cfname);

extern int ch_context_start(ch_context_t *context);

extern void ch_context_stop(ch_context_t *context);
#endif /*CH_CONTEXT_H*/

