/*
 *
 *      Filename: ch_sa_work.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-09 10:13:20
 * Last Modified: 2018-06-06 11:39:09
 */

#include "ch_sa_work.h"
#include "ch_log.h"

ch_sa_work_t * ch_sa_work_create(ch_pool_t *mp,const char *cfname){

	ch_sa_work_t *sa_work = NULL;

	ch_sa_context_t *sa_context = NULL;

	sa_work = (ch_sa_work_t*)ch_palloc(mp,sizeof(*sa_work));

	sa_work->mp = mp;

	sa_context = ch_sa_context_create(mp,cfname);

	if(sa_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create sa Context for sa work!");
		return NULL;
	}

	sa_work->sa_context = sa_context;

    /*init log*/
    ch_log_init(mp,sa_context->log_name,sa_context->log_level);
	
	sa_work->tcp_context =  ch_tcp_context_create(mp,sa_context->tcp_cfname);
	
	if(sa_work->tcp_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create tcp Context for sa work!");
		return NULL;
	}

	sa_work->pint_sa_context = ch_process_interface_sa_context_create(sa_work->mp,
		sa_context->pint_sa_cfname,0);

	if(sa_work->pint_sa_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create sa process interface context for reading!");
		return NULL;

	}

	sa_work->sat_pool = ch_sa_session_task_pool_create(sa_work);
	if(sa_work->sat_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create sa session task pool!");
		return NULL;
	}



	return sa_work;

}

int ch_sa_work_start(ch_sa_work_t *sa_work){

	/*start all sa session tasks*/
	return ch_sa_session_task_pool_start(sa_work->sat_pool);

}

void ch_sa_work_stop(ch_sa_work_t *sa_work){

	ch_sa_session_task_pool_stop(sa_work->sat_pool);

}
