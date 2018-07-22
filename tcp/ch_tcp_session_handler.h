/*
 *
 *      Filename: ch_tcp_session_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-02 10:16:36
 * Last Modified: 2018-02-02 10:16:36
 */

#ifndef CH_TCP_SESSION_HANDLER_H
#define CH_TCP_SESSION_HANDLER_H

typedef struct ch_tcp_session_handler_t ch_tcp_session_handler_t;

#include "ch_tcp_session_task.h"
#include "ch_tcp_work.h"
#include "ch_tcp_session_pool.h"
#include "ch_packet_tcp.h"
#include "ch_proto_session_store.h"

struct ch_tcp_session_handler_t {

	ch_tcp_work_t *tcp_work;

	ch_tcp_session_task_t *session_task;

	ch_tcp_session_pool_t *spool;

    ch_proto_session_store_t *pstore;
};

extern ch_tcp_session_handler_t * 
ch_tcp_session_handler_create(ch_tcp_work_t *tcp_work,ch_tcp_session_task_t *session_task);

extern int ch_tcp_session_packet_handle(ch_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,
	ch_packet_tcp_t *tcp_pkt);

#endif /*CH_TCP_SESSION_HANDLER_H*/
