/*
 *
 *      Filename: ch_udp_work.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-10 15:58:55
 * Last Modified: 2018-05-10 15:58:55
 */

#ifndef CH_UDP_WORK_H
#define CH_UDP_WORK_H

typedef struct ch_udp_work_t ch_udp_work_t;

#include "ch_udp_context.h"
#include "ch_mpool.h"
#include "ch_process_interface_udp_context.h"
#include "ch_udp_session_task_pool.h"
#include "ch_udp_app_pool.h"

struct ch_udp_work_t {

	ch_pool_t *mp;

	ch_udp_context_t *udp_context;

	ch_process_interface_udp_context_t *pint_udp_context;

	ch_udp_session_task_pool_t *udpt_pool;

	ch_udp_app_pool_t *app_pool;
};


extern ch_udp_work_t * ch_udp_work_create(ch_pool_t *mp,const char *cfname);

extern int ch_udp_work_start(ch_udp_work_t *udp_work);

extern void ch_udp_work_stop(ch_udp_work_t *udp_work);

#endif /*CH_UDP_WORK_H*/
