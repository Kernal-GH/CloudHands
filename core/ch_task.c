/*
 * =====================================================================================
 *
 *       Filename:  ch_task.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月14日 16时13分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_constants.h"
#include "ch_task.h"
#include "ch_log.h"

int ch_task_init(ch_task_t *task,void *user_data){

    if(task->init == NULL){

        ch_log(CH_LOG_ERR,"must specify task init callback!");
        return CH_ERROR;
    }

    return task->init(task,user_data);
}

int ch_task_run(ch_task_t *task,void *user_data){

    if(task->run == NULL){

        ch_log(CH_LOG_ERR,"must specify task run callback!");
        return CH_ERROR;
    }

    return task->run(task,user_data);
}

int ch_task_exit(ch_task_t *task,void *user_data){

    if(task->exit == NULL){

        ch_log(CH_LOG_ERR,"must specify task exit callback!");
        return CH_ERROR;
    }

    return task->exit(task,user_data);
}


