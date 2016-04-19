/*
 * =====================================================================================
 *
 *       Filename:  ch_rxtask.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月16日 11时04分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_RXTASK_H
#define CH_RXTASK_H

typedef struct ch_rxtask_processor_t ch_rxtask_processor_t;

typedef struct ch_rxtask_t ch_rxtask_t;

#include <rte_mempool.h>
#include <apr_tables.h>
#include "ch_task.h"
#include "ch_ipv4.h"
#include "ch_ethertype.h"
#include "ch_packet.h"
#include "ch_tcp.h"

struct ch_rxtask_t {

    ch_task_t task;
    ch_packet_info_t pinfo;
    struct rte_mempool *pktmbuf_pool;
    apr_array_header_t *rx_ports;
    apr_array_header_t *pkt_processors;

    ch_ethertype_t * ethertype;
    ch_ipv4_t *ipv4;
    ch_tcp_t  *tcp;
};

struct ch_rxtask_processor_t {
    const char *name;
    int (*process)(ch_port_t *port,struct rte_mbuf *mbuf,uint64_t time,void *user_data);
    void *user_data;
};

extern ch_task_t * ch_rxtask_create(ch_context_t *context,unsigned int rx_port_mask);

extern void ch_rxtask_pkt_processor_register(ch_rxtask_t *task,ch_rxtask_processor_t *prosessor);

#endif /*CH_RXTASK_H*/

