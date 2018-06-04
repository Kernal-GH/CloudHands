/*
 *
 *      Filename: ch_pp_work.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 17:00:16
 * Last Modified: 2018-05-14 17:00:16
 */

#ifndef CH_PP_WORK_H
#define CH_PP_WORK_H

typedef struct ch_pp_work_t ch_pp_work_t;

#include "ch_pp_context.h"
#include "ch_pp_pool.h"
#include "ch_shm_format.h"

struct ch_pp_work_t {

	ch_pool_t *mp;

	ch_pp_context_t *pcontext;

	ch_pp_pool_t *pp_pool;

	ch_shm_format_t *shm_fmt;

};


extern ch_pp_work_t * ch_pp_work_create(ch_pool_t *mp,const char *cfname,const char *shm_fname);

extern int ch_pp_work_start(ch_pp_work_t *pwork);

extern void ch_pp_work_stop(ch_pp_work_t *pwork);


#endif /*CH_PP_WORK_H*/
