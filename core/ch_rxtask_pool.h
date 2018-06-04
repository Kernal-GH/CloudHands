/*
 *
 *      Filename: ch_rxtask_pool.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-08-03 00:56:16
 * Last Modified: 2016-08-03 00:56:16
 */

#ifndef CH_TXTASK_POOL_H
#define CH_TXTASK_POOL_H

typedef struct ch_rxtask_processor_t ch_rxtask_processor_t;
typedef struct ch_rxtask_pool_t ch_rxtask_pool_t;

#include "ch_context.h"
#include "ch_ethertype.h"
#include "ch_ipv4.h"
#include "ch_tcp.h"
#include "ch_udp.h"

struct ch_rxtask_pool_t {

	ch_context_t *context;

	apr_array_header_t *rxtasks;

	apr_array_header_t *pkt_processors;

    ch_ethertype_t * ethertype;
    ch_ipv4_t *ipv4;
    ch_tcp_t  *tcp;
	ch_udp_t  *udp;
};

struct ch_rxtask_processor_t {
    const char *name;
    int (*process)(struct rte_mbuf *mbuf,uint64_t time,void *priv_data);
    void *priv_data;
};



static inline void ch_rxtask_pool_pkt_processor_register(ch_rxtask_pool_t *rxtp,ch_rxtask_processor_t *prosessor){

    *(ch_rxtask_processor_t**)apr_array_push(rxtp->pkt_processors) = prosessor;
}


extern uint64_t ch_rxtask_pool_packets_get(ch_rxtask_pool_t *rxtp);

extern void ch_rxtask_pool_dump(ch_rxtask_pool_t *rxtp);

extern ch_rxtask_pool_t * ch_rxtask_pool_create(ch_context_t *context);

#endif /* CH_TXTASK_POOL_H */
