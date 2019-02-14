/*
 *
 *      Filename: ch_sa_udp_session_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 15:31:17
 * Last Modified: 2018-04-13 15:31:17
 */

#ifndef CH_SA_UDP_SESSION_HANDLER_H
#define CH_SA_UDP_SESSION_HANDLER_H

typedef struct ch_sa_udp_session_handler_t ch_sa_udp_session_handler_t;

#include "ch_sa_udp_session_pool.h"
#include "ch_sa_session_task.h"
#include "ch_sa_work.h"

struct ch_sa_udp_session_handler_t {


	ch_sa_work_t *sa_work;

	ch_sa_session_task_t *session_task;

	ch_sa_udp_session_pool_t *udp_pool;


};


extern ch_sa_udp_session_handler_t *
ch_sa_udp_session_handler_create(ch_sa_work_t *sa_work,ch_sa_session_task_t *session_task);

extern int
ch_sa_udp_session_packet_handle(ch_sa_udp_session_handler_t *udp_handler,ch_packet_t *pkt);


#endif /*CH_SA_UDP_SESSION_HANDLER_H*/
