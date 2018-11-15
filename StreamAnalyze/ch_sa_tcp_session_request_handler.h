/*
 *
 *      Filename: ch_sa_tcp_session_request_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-21 09:54:09
 * Last Modified: 2018-03-21 09:54:09
 */

#ifndef CH_SA_TCP_SESSION_REQUEST_H
#define CH_SA_TCP_SESSION_REQUEST_H


typedef struct ch_sa_tcp_session_request_handler_t ch_sa_tcp_session_request_handler_t;

#include "ch_sa_work.h"
#include "ch_tcp_session_request_pool.h"
#include "ch_packet.h"
#include "ch_sa_session_task.h"

struct ch_sa_tcp_session_request_handler_t {


	ch_sa_work_t *sa_work;

	ch_tcp_session_request_pool_t *req_pool;

	ch_sa_session_task_t *session_task;

};

#define sa_tcp_session_pool_get(req_handler) (req_handler->session_task->tcp_session_handler->tcp_session_pool)

extern ch_sa_tcp_session_request_handler_t *
ch_sa_tcp_session_request_handler_create(ch_sa_work_t *sa_work,ch_sa_session_task_t *session_task);


extern int ch_sa_tcp_session_request_packet_handle(ch_sa_tcp_session_request_handler_t *tcp_req_handler,ch_packet_t *pkt);

#endif /*CH_SA_TCP_SESSION_REQUEST_H*/
