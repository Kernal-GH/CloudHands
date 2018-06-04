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


typedef struct ch_rxtask_t ch_rxtask_t;

#include "ch_port.h"
#include "ch_task.h"

struct ch_rxtask_t {

    ch_task_t task;
    
    ch_context_t *context;

	ch_port_queue_t *pq;

	uint64_t rx_packets;
};

extern ch_task_t * ch_rxtask_create(ch_context_t *context,unsigned int id);

#endif /*CH_RXTASK_H*/

