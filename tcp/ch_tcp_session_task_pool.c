/*
 *
 *      Filename: ch_tcp_session_task_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-05 17:00:40
 * Last Modified: 2018-04-04 17:08:37
 */

#include "ch_tcp_session_task_pool.h"
#include "ch_log.h"

ch_tcp_session_task_pool_t * ch_tcp_session_task_pool_create(ch_tcp_work_t *tcp_work){



	int i;
	ch_tcp_session_task_t *ttask;

	ch_tcp_context_t *tcp_context = tcp_work->tcp_context;

	ch_tcp_session_task_pool_t *tstpool = (ch_tcp_session_task_pool_t*)ch_palloc(tcp_work->mp,sizeof(*tstpool));

	tstpool->cpool = ch_core_pool_create(tcp_work->mp,NULL);
	
	if(tstpool->cpool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create cpu core pool for tcp session task pool!");
		return NULL;
	}

	tstpool->tsession_tasks = ch_array_make(tcp_work->mp,16,sizeof(ch_tcp_session_task_t*));

	/*create all tcp session task*/
	for(i = 0;i<tcp_context->tasks_n;i++){
	

		ttask = ch_tcp_session_task_create(tcp_work,i+1);
		if(ttask == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot crate tcp session task:%d",i+1);

			return NULL;
		}

		/*bind to a cpu core*/
	   if(ch_core_pool_bind_task(tstpool->cpool,(ch_task_t*)ttask)){
	   
		  /* Bind error */
		  ch_log(CH_LOG_ERR,"Cannot bind this tcp session task:%d into a cpu core!",i+1);
		  return NULL;
	   }

		/*add to task pool*/
		*(ch_tcp_session_task_t**)ch_array_push(tstpool->tsession_tasks) = ttask;

	}

	return tstpool;
}

int ch_tcp_session_task_pool_start(ch_tcp_session_task_pool_t *tstpool){

	/*start all tcp session task!*/
	if(ch_core_pool_slaves_setup(tstpool->cpool)){
	
		ch_log(CH_LOG_ERR,"setup all tcp session task failed!");
		return -1;
	}

	ch_core_pool_wait_for_slaves(tstpool->cpool);
	/*ok!*/

	return 0;

}

void ch_tcp_session_task_pool_stop(ch_tcp_session_task_pool_t *tstpool ch_unused){


}
