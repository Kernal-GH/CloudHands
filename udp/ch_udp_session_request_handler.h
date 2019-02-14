/*
 *
 *      Filename: ch_udp_session_request_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-12 17:59:26
 * Last Modified: 2018-09-12 17:59:26
 */

#ifndef CH_UDP_SESSION_REQUEST_HANDLER_H
#define CH_UDP_SESSION_REQUEST_HANDLER_H

typedef struct ch_udp_session_request_handler_t ch_udp_session_request_handler_t;

#include "ch_udp_session_request_pool.h"
#include "ch_udp_session_task.h"
#include "ch_udp_work.h"

struct ch_udp_session_request_handler_t {


	ch_udp_work_t *udp_work;

	ch_udp_session_task_t *session_task;

	ch_udp_session_request_pool_t *req_pool;

};

#define CH_UDP_SESSION_POOL_GET(handler) ((handler)->session_task->udp_session_handler->udp_pool)

extern ch_udp_session_request_handler_t *
ch_udp_session_request_handler_create(ch_udp_work_t *udp_work,ch_udp_session_task_t *session_task);

extern int
ch_udp_session_request_packet_handle(ch_udp_session_request_handler_t *udp_handler,ch_packet_t *pkt);


#endif /*CH_UDP_SESSION_REQUEST_HANDLER_H*/
