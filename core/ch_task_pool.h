/*
 * =====================================================================================
 *
 *       Filename:  ch_task_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月25日 14时45分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_TASK_POOL_H
#define CH_TASK_POOL_H

typedef struct ch_task_pool_t ch_task_pool_t;
typedef struct ch_task_t ch_task_t;

#include "ch_context.h"
#include "ch_core_pool.h"

#define TASK_RETURN_OK 0
#define TASK_RETURN_EXIT -1
#define TASK_RETURN_SLEEP -2

struct ch_task_pool_t {

    ch_context_t *context;
    apr_array_header_t *rx_tsks;
    apr_array_header_t *as_tsks;
};


struct ch_task_t {

    ch_core_t *core;
    void *priv_data;
    unsigned int tsk_id;

    int (*init)(ch_task_t *task,void *priv_data);
    int (*run)(ch_task_t *task,void *priv_data);
    int (*exit)(ch_task_t *task,void *priv_data);
};

extern ch_task_pool_t * ch_task_pool_create(ch_context_t *context);

extern int  ch_task_pool_task_run(ch_task_pool_t *tpool,ch_task_t *tsk);

#endif /*CH_TASK_POOL_H*/

