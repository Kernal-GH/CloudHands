/*
 *
 *      Filename: ch_udp_session_task_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-09 09:40:01
 * Last Modified: 2018-05-10 16:36:23
 */

#include "ch_udp_session_task_pool.h"
#include "ch_log.h"

ch_udp_session_task_pool_t * ch_udp_session_task_pool_create(ch_udp_work_t *udp_work){

	int i;
	ch_udp_session_task_t *udp_task;

	ch_udp_context_t *udp_context = udp_work->udp_context;

	ch_udp_session_task_pool_t *udpt_pool = (ch_udp_session_task_pool_t*)ch_palloc(udp_work->mp,sizeof(*udpt_pool));

	udpt_pool->cpool = ch_core_pool_create(udp_work->mp,NULL);
	
	if(udpt_pool->cpool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create cpu core pool for udp session task pool!");
		return NULL;
	}

	udpt_pool->udp_session_tasks = ch_array_make(udp_work->mp,16,sizeof(ch_udp_session_task_t*));

	/*create all udp session task*/
	for(i = 0;i<udp_context->tasks_n;i++){
	

		udp_task = ch_udp_session_task_create(udp_work,i+1);
		if(udp_task == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot crate udp session task:%d",i+1);

			return NULL;
		}

		/*bind to a cpu core*/
	   if(ch_core_pool_bind_task(udpt_pool->cpool,(ch_task_t*)udp_task)){
	   
		  /* Bind error */
		  ch_log(CH_LOG_ERR,"Cannot bind this udp session task:%d into a cpu core!",i+1);
		  return NULL;
	   }

		/*add to task pool*/
		*(ch_udp_session_task_t**)ch_array_push(udpt_pool->udp_session_tasks) = udp_task;

	}

	return udpt_pool;
}

int ch_udp_session_task_pool_start(ch_udp_session_task_pool_t *udpt_pool){

	/*start all udp session task!*/
	if(ch_core_pool_slaves_setup(udpt_pool->cpool)){
	
		ch_log(CH_LOG_ERR,"setup all udp session task failed!");
		return -1;
	}

	ch_core_pool_wait_for_slaves(udpt_pool->cpool);
	/*ok!*/

	return 0;

}

void ch_udp_session_task_pool_stop(ch_udp_session_task_pool_t *udpt_pool ch_unused){


}
