/*
 *
 *      Filename: ch_sa_session_task.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-21 10:14:09
 * Last Modified: 2018-03-21 10:14:09
 */

#ifndef CH_SA_SESSION_TASK_H
#define CH_SA_SESSION_TASK_H

typedef struct ch_sa_session_task_t ch_sa_session_task_t;

#include "ch_task.h"
#include "ch_sa_work.h"
#include "ch_sa_tcp_session_request_handler.h"
#include "ch_sa_tcp_session_handler.h"
#include "ch_sa_udp_session_handler.h"
#include "ch_shm_format.h"
#include "ch_buffer.h"
#include "ch_sa_data_store_pool.h"
#include "ch_lua_engine.h"

struct ch_sa_session_task_t {

	ch_task_t task;

	ch_sa_work_t *sa_work;

	ch_process_queue_t *pqueue;

	ch_sa_tcp_session_request_handler_t *tcp_req_handler;
	ch_sa_tcp_session_handler_t *tcp_session_handler;

	ch_sa_udp_session_handler_t *udp_handler;

	ch_shm_format_t *shm_fmt;

	ch_buffer_t sa_buffer;

	ch_sa_data_store_pool_t *dstore_pool;

    ch_lua_engine_t *lua_engine;

};

extern ch_sa_session_task_t *
ch_sa_session_task_create(ch_sa_work_t *sa_work,uint32_t task_id);

#endif /*CH_SA_SESSION_TASK_H*/
