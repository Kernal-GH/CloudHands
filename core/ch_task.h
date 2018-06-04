/*
 *
 *      Filename: ch_task.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-08-03 02:59:39
 * Last Modified: 2016-08-03 02:59:39
 */

#ifndef CH_TASK_H
#define CH_TASK_H

typedef struct ch_task_t ch_task_t;

#include "ch_core_pool.h"

#define TASK_RETURN_OK 0
#define TASK_RETURN_EXIT -1
#define TASK_RETURN_SLEEP -2

struct ch_task_t {

    ch_core_t *core;
    void *priv_data;
    unsigned int tsk_id;

    int (*init)(ch_task_t *task,void *priv_data);
    int (*run)(ch_task_t *task,void *priv_data);
    int (*exit)(ch_task_t *task,void *priv_data);
};

extern int  ch_task_run(ch_task_t *tsk);

#endif /* CH_TASK_H */
