/*
 *
 *      Filename: ch_sa_session_task_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-09 09:40:01
 * Last Modified: 2018-04-09 10:07:45
 */

#include "ch_sa_session_task_pool.h"
#include "ch_log.h"

ch_sa_session_task_pool_t * ch_sa_session_task_pool_create(ch_sa_work_t *sa_work){

	int i;
	ch_sa_session_task_t *sa_task;

	ch_sa_context_t *sa_context = sa_work->sa_context;

	ch_sa_session_task_pool_t *sat_pool = (ch_sa_session_task_pool_t*)ch_palloc(sa_work->mp,sizeof(*sat_pool));

	sat_pool->cpool = ch_core_pool_create(sa_work->mp,NULL);
	
	if(sat_pool->cpool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create cpu core pool for sa session task pool!");
		return NULL;
	}

	sat_pool->sa_session_tasks = ch_array_make(sa_work->mp,16,sizeof(ch_sa_session_task_t*));

	/*create all sa session task*/
	for(i = 0;i<sa_context->tasks_n;i++){
	

		sa_task = ch_sa_session_task_create(sa_work,i+1);
		if(sa_task == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot crate sa session task:%d",i+1);

			return NULL;
		}

		/*bind to a cpu core*/
	   if(ch_core_pool_bind_task(sat_pool->cpool,(ch_task_t*)sa_task)){
	   
		  /* Bind error */
		  ch_log(CH_LOG_ERR,"Cannot bind this sa session task:%d into a cpu core!",i+1);
		  return NULL;
	   }

		/*add to task pool*/
		*(ch_sa_session_task_t**)ch_array_push(sat_pool->sa_session_tasks) = sa_task;

	}

	return sat_pool;
}

int ch_sa_session_task_pool_start(ch_sa_session_task_pool_t *sat_pool){

	/*start all sa session task!*/
	if(ch_core_pool_slaves_setup(sat_pool->cpool)){
	
		ch_log(CH_LOG_ERR,"setup all sa session task failed!");
		return -1;
	}

	ch_core_pool_wait_for_slaves(sat_pool->cpool);
	/*ok!*/

	return 0;

}

void ch_sa_session_task_pool_stop(ch_sa_session_task_pool_t *sat_pool ch_unused){


}
