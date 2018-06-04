/*
 *
 *      Filename: ch_udp_session_task_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-08 19:21:47
 * Last Modified: 2018-04-08 19:21:47
 */

#ifndef CH_UDP_SESSION_TASK_POOL_H
#define CH_UDP_SESSION_TASK_POOL_H

typedef struct ch_udp_session_task_pool_t ch_udp_session_task_pool_t;

#include "ch_udp_work.h"
#include "ch_udp_session_task.h"
#include "ch_tables.h"
#include "ch_core_pool.h"

struct ch_udp_session_task_pool_t {

	ch_core_pool_t *cpool;

	ch_array_header_t *udp_session_tasks;

};


extern ch_udp_session_task_pool_t * ch_udp_session_task_pool_create(ch_udp_work_t *udp_work);

extern int ch_udp_session_task_pool_start(ch_udp_session_task_pool_t *udpt_pool);

extern void ch_udp_session_task_pool_stop(ch_udp_session_task_pool_t *udpt_pool);

#endif /*CH_UDP_SESSION_TASK_POOL_H*/
