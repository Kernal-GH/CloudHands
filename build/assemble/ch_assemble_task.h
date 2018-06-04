/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月16日 18时01分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_ASSEMBLE_TASK_H
#define CH_ASSEMBLE_TASK_H

typedef struct ch_assemble_task_t ch_assemble_task_t;
#include <limits.h>
#include <apr_pools.h>
#include <rte_ring.h>
#include "ch_util.h"
#include "ch_assemble.h"
#include "ch_assemble_packet.h"

struct ch_assemble_task_t {

    ch_task_t task;
    ch_assemble_t *as;
    ch_context_t *context;
    struct rte_ring *pkts;

	size_t no_packets_count;

    unsigned int task_id;
    size_t ring_size;

    struct {

        uint64_t packets;
        uint64_t sessions;
        uint64_t last_time;
    }stats;
};

static inline void ch_assemble_task_stats_update(ch_assemble_task_t *tk,uint64_t pkts,uint64_t ss){

    tk->stats.packets = (tk->stats.packets+pkts)%ULONG_MAX;
    tk->stats.sessions = (tk->stats.sessions+ss)%ULONG_MAX;
    tk->stats.last_time = ch_get_current_timems()/1000; 
}


extern ch_task_t * ch_assemble_task_create(ch_context_t *context,ch_assemble_t *as,const char *prefix,unsigned int task_id);


extern int ch_assemble_task_pkt_put(ch_assemble_task_t *astask,ch_assemble_packet_t *as_pkt);


#endif /*CH_ASSEMBLE_TASK_H*/

