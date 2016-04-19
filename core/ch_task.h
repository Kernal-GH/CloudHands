/*
 * =====================================================================================
 *
 *       Filename:  ch_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月16日 10时52分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_TASK_H
#define CH_TASK_H

typedef struct ch_task_t ch_task_t;

#include "ch_core.h"

struct ch_task_t {

    ch_core_t *core;

    int (*init)(ch_task_t *task,void *user_data);
    int (*run)(ch_task_t *task,void *user_data);
    int (*exit)(ch_task_t *task,void *user_data);
};

extern int ch_task_init(ch_task_t *task,void *user_data);

extern int ch_task_run(ch_task_t *task,void *user_data);

extern int ch_task_exit(ch_task_t *task,void *user_data);

#endif /*CH_TASK_H*/

