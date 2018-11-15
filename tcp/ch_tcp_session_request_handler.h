/*
 *
 *      Filename: ch_tcp_session_request_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-02 10:15:09
 * Last Modified: 2018-02-02 10:15:09
 */

#ifndef CH_TCP_SESSION_REQUEST_HANDLER_H
#define CH_TCP_SESSION_REQUEST_HANDLER_H

typedef struct ch_tcp_session_request_handler_t ch_tcp_session_request_handler_t;

#include "ch_tcp_session_task.h"
#include "ch_tcp_session_request_pool.h"
#include "ch_tcp_work.h"
#include "ch_packet.h"

struct ch_tcp_session_request_handler_t {

	ch_tcp_session_task_t *session_task;

	ch_tcp_work_t *tcp_work;

	ch_tcp_session_request_pool_t *req_pool;

	uint64_t cur_session_id;
};

#define session_pool_get(req_handler) (req_handler->session_task->shandler->spool)

extern ch_tcp_session_request_handler_t *
ch_tcp_session_request_handler_create(ch_tcp_work_t *tcp_work,ch_tcp_session_task_t *session_task);

extern int ch_tcp_session_request_packet_handle(ch_tcp_session_request_handler_t *req_handler,ch_packet_tcp_t *tcp_pkt);

#endif /*CH_TCP_SESSION_REQUEST_HANDLER_H*/
