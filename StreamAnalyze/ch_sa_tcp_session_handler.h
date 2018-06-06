/*
 *
 *      Filename: ch_sa_tcp_session_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-21 10:39:59
 * Last Modified: 2018-03-21 10:39:59
 */

#ifndef CH_SA_TCP_SESSION_HANDLER_H
#define CH_SA_TCP_SESSION_HANDLER_H

typedef struct ch_sa_tcp_session_handler_t ch_sa_tcp_session_handler_t;

#include "ch_tcp_session_pool.h"
#include "ch_sa_session_task.h"
#include "ch_sa_work.h"
#include "ch_sa_session_entry.h"

struct ch_sa_tcp_session_handler_t {

	ch_sa_work_t *sa_work;

	ch_sa_session_task_t *session_task;
	
	ch_tcp_session_pool_t *tcp_session_pool;

};

#define ch_sa_session_tcp_entry_get(shandler,tcp_session)(ch_sa_session_entry_t*)ch_ptable_entry_priv_data(shandler->tcp_session_pool->tcp_session_tbl,(void*)tcp_session)

extern ch_sa_tcp_session_handler_t *
ch_sa_tcp_session_handler_create(ch_sa_work_t *sa_work,ch_sa_session_task_t *session_task);

extern int
ch_sa_tcp_session_packet_handle(ch_sa_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,
	ch_packet_tcp_t *tcp_pkt,
	int is_new_create);

#endif /*CH_SA_TCP_SESSION_HANDLER_H*/
