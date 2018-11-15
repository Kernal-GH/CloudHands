/*
 *
 *      Filename: ch_tcp_session_task_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-05 16:45:19
 * Last Modified: 2018-02-05 16:45:19
 */

#ifndef CH_TCP_SESSION_TASK_POOL_H
#define CH_TCP_SESSION_TASK_POOL_H

typedef struct ch_tcp_session_task_pool_t ch_tcp_session_task_pool_t;

#include "ch_tcp_work.h"
#include "ch_tcp_session_task.h"
#include "ch_tables.h"
#include "ch_core_pool.h"

struct ch_tcp_session_task_pool_t {

	ch_core_pool_t *cpool;

	ch_array_header_t *tsession_tasks;

};


extern ch_tcp_session_task_pool_t * ch_tcp_session_task_pool_create(ch_tcp_work_t *tcp_work);

extern int ch_tcp_session_task_pool_start(ch_tcp_session_task_pool_t *tstpool);

extern void ch_tcp_session_task_pool_stop(ch_tcp_session_task_pool_t *tstpool);

#endif /*CH_TCP_SESSION_TASK_POOL_H*/
