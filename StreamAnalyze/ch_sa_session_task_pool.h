/*
 *
 *      Filename: ch_sa_session_task_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-08 19:21:47
 * Last Modified: 2018-04-08 19:21:47
 */

#ifndef CH_SA_SESSION_TASK_POOL_H
#define CH_SA_SESSION_TASK_POOL_H

typedef struct ch_sa_session_task_pool_t ch_sa_session_task_pool_t;

#include "ch_sa_work.h"
#include "ch_sa_session_task.h"
#include "ch_tables.h"
#include "ch_core_pool.h"

struct ch_sa_session_task_pool_t {

	ch_core_pool_t *cpool;

	ch_array_header_t *sa_session_tasks;

};


extern ch_sa_session_task_pool_t * ch_sa_session_task_pool_create(ch_sa_work_t *sa_work);

extern int ch_sa_session_task_pool_start(ch_sa_session_task_pool_t *sat_pool);

extern void ch_sa_session_task_pool_stop(ch_sa_session_task_pool_t *sat_pool);

#endif /*CH_SA_SESSION_TASK_POOL_H*/
