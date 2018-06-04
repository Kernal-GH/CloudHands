/*
 *
 *      Filename: ch_task.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-08-03 03:02:16
 * Last Modified: 2016-08-03 20:05:55
 */


#include "ch_log.h"
#include "ch_constants.h"
#include "ch_context.h"
int  ch_task_run(ch_task_t *tsk){

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
