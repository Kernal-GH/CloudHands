/*
 *
 *      Filename: ch_tcp_work.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-04 16:25:49
 * Last Modified: 2018-04-04 16:25:49
 */

#ifndef CH_TCP_WORK_H
#define CH_TCP_WORK_H

typedef struct ch_tcp_work_t ch_tcp_work_t;

#include "ch_mpool.h"
#include "ch_process_interface_tcp_context.h"
#include "ch_tcp_session_task_pool.h"
#include "ch_tcp_context.h"
#include "ch_tcp_app_pool.h"

struct ch_tcp_work_t {

	ch_pool_t *mp;

	ch_tcp_context_t *tcp_context;

	ch_process_interface_tcp_context_t *pint_context;

	ch_tcp_app_pool_t *ta_pool;

	ch_tcp_session_task_pool_t *tstpool;

};


extern ch_tcp_work_t *
ch_tcp_work_create(ch_pool_t *mp,const char *cfname);

extern int ch_tcp_work_start(ch_tcp_work_t *tcp_work);

extern void ch_tcp_work_stop(ch_tcp_work_t *tcp_work);

#endif /*CH_TCP_WORK_H*/
