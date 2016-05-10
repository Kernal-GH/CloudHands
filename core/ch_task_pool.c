/*
 * =====================================================================================
 *
 *       Filename:  ch_task_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月25日 16时07分48秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_task_pool.h"
#include "ch_rxtask.h"
#include "ch_assemble_task.h"
#include "ch_log.h"
#include "ch_constants.h"

static inline void _task_put(apr_array_header_t *arr,ch_task_t *tsk){

    *(ch_task_t**)apr_array_push(arr) = tsk; 
}

static int _rx_tasks_create(ch_task_pool_t *tpool){

	if(tpool->context->ppool == NULL)
		return CH_OK;

    ch_task_t *tsk = ch_rxtask_create(tpool->context);
    if(tsk == NULL){
        ch_log(CH_LOG_ERR,"create recieve task failed!");
        return CH_ERROR;
    }

    _task_put(tpool->rx_tsks,tsk);

    return CH_OK;
}

static int _as_tasks_create(ch_task_pool_t *tpool){

    ch_task_t *tsk;
    int i;

    for(i = 0; i<tpool->context->n_assemble_tasks; i++){
        
        tsk = ch_assemble_task_create(tpool->context,i+1);
        if(tsk == NULL){
            ch_log(CH_LOG_ERR,"Create assemble task[%d] failed!",i+1);
            return CH_ERROR;
        }

        _task_put(tpool->as_tsks,tsk);
    }

    return CH_OK;
}

int  ch_task_pool_task_run(ch_task_pool_t *tpool,ch_task_t *tsk){

    int rc;

    if(tsk->init){
        if(CH_ERROR == tsk->init(tsk,tsk->priv_data)){
            ch_log(CH_LOG_ERR,"Init task failed before running!");
            return CH_ERROR;
        }
    }

    ch_log(CH_LOG_INFO,"Start to run task[%u] in cpu core[%u].",tsk->tsk_id,tsk->core->core_id);

    while(1){

        rc = tsk->run(tsk,tsk->priv_data);
        
        if(rc == TASK_RETURN_EXIT){

            ch_log(CH_LOG_ERR,"Task[%u] will be exit!",tsk->tsk_id);
            if(tsk->exit){
                tsk->exit(tsk,tsk->priv_data);
            }

            return CH_ERROR;
        }

        if(rc == TASK_RETURN_SLEEP){
            usleep(5);
        }
    }//while

}

ch_task_pool_t * ch_task_pool_create(ch_context_t *context){

    ch_task_t *tsk;

    apr_pool_t *mp = context->mp;

    ch_task_pool_t *tpool = (ch_task_pool_t*)apr_palloc(mp,sizeof(ch_task_pool_t));
    
    tpool->context = context;
    tpool->rx_tsks = apr_array_make(context->mp,16,sizeof(ch_task_t*));
    tpool->as_tsks = apr_array_make(context->mp,16,sizeof(ch_task_t*));

    /* create all rx tasks */
    if(_rx_tasks_create(tpool) == CH_ERROR){
        return NULL;
    }

    if(_as_tasks_create(tpool) == CH_ERROR){

        return NULL;
    }

    /*ok!*/
    
    ch_log(CH_LOG_INFO,"create recieve tasks[%u] and assemble tasks[%u] in task pool ok!",
            tpool->rx_tsks->nelts,tpool->as_tsks->nelts);

    return tpool;
}

