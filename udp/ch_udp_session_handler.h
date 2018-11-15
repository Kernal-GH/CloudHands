/*
 *
 *      Filename: ch_udp_udp_session_handler.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 15:31:17
 * Last Modified: 2018-04-13 15:31:17
 */

#ifndef CH_UDP_SESSION_HANDLER_H
#define CH_UDP_SESSION_HANDLER_H

typedef struct ch_udp_session_handler_t ch_udp_session_handler_t;

#include "ch_udp_session_pool.h"
#include "ch_udp_session.h"
#include "ch_udp_session_task.h"
#include "ch_udp_work.h"

struct ch_udp_session_handler_t {


	ch_udp_work_t *udp_work;

	ch_udp_session_task_t *session_task;

	ch_udp_session_pool_t *udp_pool;

};


extern ch_udp_session_handler_t *
ch_udp_session_handler_create(ch_udp_work_t *udp_work,ch_udp_session_task_t *session_task);

extern int
ch_udp_session_packet_handle(ch_udp_session_handler_t *udp_handler,
	ch_udp_session_t *udp_session,ch_packet_udp_t *pkt_udp,int is_new_created);


#endif /*CH_UDP_SESSION_HANDLER_H*/
