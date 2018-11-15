/*
 *
 *      Filename: ch_packet_rxtask_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-08 15:04:01
 * Last Modified: 2017-11-08 15:04:01
 */

#ifndef CH_PACKET_RXTASK_POOL_H
#define CH_PACKET_RXTASK_POOL_H

typedef struct ch_packet_rxtask_pool_t ch_packet_rxtask_pool_t;

#include "ch_tables.h"
#include "ch_pdispatcher_context.h"

struct ch_packet_rxtask_pool_t {

	ch_pdispatcher_context_t *pdcontext;
	ch_array_header_t *rxtasks;

	int rxtasks_n;

	int per_port_queue_n;
	int max_port_queue_n;
};


extern ch_packet_rxtask_pool_t *ch_packet_rxtask_pool_create(ch_pdispatcher_context_t *pdcontext);

#endif /*CH_PACKET_RXTASK_POOL_H*/
