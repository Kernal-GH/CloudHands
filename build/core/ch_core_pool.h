/*
 * =====================================================================================
 *
 *       Filename:  ch_core_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月25日 11时39分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_CORE_POOL_H
#define CH_CORE_POOL_H

typedef struct ch_core_pool_t ch_core_pool_t;
typedef struct ch_core_t ch_core_t;

#include <apr_pools.h>
#include <apr_tables.h>
#include "ch_context.h"
#include "ch_task_pool.h"

struct ch_core_pool_t {

    ch_context_t *context;
    apr_array_header_t *cores;
};

struct ch_core_t {

    ch_task_t *tsk;
    unsigned int core_id;
    unsigned int socket;
    unsigned bound:1;
};

extern ch_core_pool_t * ch_core_pool_create(ch_context_t *context);

extern int ch_core_pool_task_bind(ch_core_pool_t *cpool,ch_task_t *tsk);

extern void ch_core_pool_task_unbind(ch_core_pool_t *cpool,ch_core_t *core);

extern void ch_core_pool_cores_setup(ch_core_pool_t *cpool);
#endif /*CH_CORE_POOL_H*/

