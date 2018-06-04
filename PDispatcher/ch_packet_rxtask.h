/*
 * =====================================================================================
 *
 *       Filename:  ch_packet_rxtask.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2017 10:01:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_PACKET_RXTASK_H
#define CH_PACKET_RXTASK_H

typedef struct ch_packet_rxtask_t ch_packet_rxtask_t;

#include "ch_list.h"
#include "ch_task.h"
#include "ch_port_pool.h"
#include "ch_pdispatcher_context.h"

struct ch_packet_rxtask_t {

   ch_task_t task;

   struct list_head port_queues;

   struct rte_mbuf **rx_pkts_burst;

   ch_pdispatcher_context_t *pdcontext;

   uint64_t last_dump_time;

   uint32_t socket;

};

extern ch_task_t * ch_packet_rxtask_create(ch_pdispatcher_context_t *pdcontext,
  uint32_t task_id,int queue_n);

#endif /*CH_PACKET_RXTASK_H*/

