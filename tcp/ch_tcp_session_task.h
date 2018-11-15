/*
 *
 *      Filename: ch_tcp_session_task.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-02 10:07:23
 * Last Modified: 2018-02-02 10:07:23
 */

#ifndef CH_TCP_SESSION_TASK_H
#define CH_TCP_SESSION_TASK_H

typedef struct ch_tcp_session_task_t ch_tcp_session_task_t;

#include "ch_task.h"
#include "ch_tcp_work.h"
#include "ch_process_interface.h"
#include "ch_tcp_session_handler.h"
#include "ch_tcp_session_request_handler.h"

struct ch_tcp_session_task_t {

	ch_task_t task;
	ch_tcp_work_t *tcp_work;

	ch_process_queue_t *pqueue;

	ch_tcp_session_handler_t *shandler;

	ch_tcp_session_request_handler_t *sreq_handler;

};

#define PROCESSOR_RET_OK 0
#define PROCESSOR_RET_DROP -1

extern ch_tcp_session_task_t * ch_tcp_session_task_create(ch_tcp_work_t *tcp_work,uint32_t task_id);
#endif /*CH_TCP_SESSION_TASK_H*/
