/*
 *
 *      Filename: ch_sa_work.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-04 17:47:13
 * Last Modified: 2018-04-04 17:47:13
 */

#ifndef CH_SA_WORK_H
#define CH_SA_WORK_H

typedef struct ch_sa_work_t ch_sa_work_t;

#include "ch_sa_context.h"
#include "ch_mpool.h"
#include "ch_tcp_context.h"
#include "ch_process_interface_sa_context.h"
#include "ch_sa_session_task_pool.h"

struct ch_sa_work_t {

	ch_pool_t *mp;

	ch_sa_context_t *sa_context;

	ch_tcp_context_t *tcp_context;

	ch_process_interface_sa_context_t *pint_sa_context;

	ch_sa_session_task_pool_t *sat_pool;

};


extern ch_sa_work_t * ch_sa_work_create(ch_pool_t *mp,const char *cfname);


extern int ch_sa_work_start(ch_sa_work_t *sa_work);

extern void ch_sa_work_stop(ch_sa_work_t *sa_work);

#endif /*CH_SA_WORK_H*/
