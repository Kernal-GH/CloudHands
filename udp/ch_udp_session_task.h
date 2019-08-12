/*
 *
 *      Filename: ch_udp_session_task.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-21 10:14:09
 * Last Modified: 2018-03-21 10:14:09
 */

#ifndef CH_UDP_SESSION_TASK_H
#define CH_UDP_SESSION_TASK_H

typedef struct ch_udp_session_task_t ch_udp_session_task_t;

#include "ch_task.h"
#include "ch_udp_work.h"
#include "ch_udp_session_handler.h"
#include "ch_udp_session_request_handler.h"
#include "ch_shm_format.h"
#include "ch_data_output.h"
#include "ch_msgpack_store.h"

struct ch_udp_session_task_t {

	ch_task_t task;

	ch_udp_work_t *udp_work;

	ch_process_queue_t *pqueue;

	ch_udp_session_handler_t *udp_session_handler;
	
	ch_udp_session_request_handler_t *udp_session_req_handler;

	ch_shm_format_t *shm_fmt;

	ch_data_output_t dout;

    ch_msgpack_store_t *dstore;

};

extern ch_udp_session_task_t *
ch_udp_session_task_create(ch_udp_work_t *udp_work,uint32_t task_id);

#define ch_udp_session_task_handlers_timeout_free(task) do { \
	if(ch_plist_entries_timeout_free(task->udp_session_req_handler->req_pool->plist,NULL)){ \
		ch_plist_dump(task->udp_session_req_handler->req_pool->plist,stdout); \
	} \
	if(ch_ptable_entries_timeout_free(task->udp_session_handler->udp_pool->udp_session_tbl,NULL)){ \
		ch_ptable_dump(task->udp_session_handler->udp_pool->udp_session_tbl,stdout); \
	} \
}while(0)

#endif /*CH_UDP_SESSION_TASK_H*/
