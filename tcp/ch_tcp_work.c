/*
 *
 *      Filename: ch_tcp_work.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-04 16:42:26
 * Last Modified: 2018-04-13 10:48:20
 */

#include "ch_tcp_work.h"
#include "ch_log.h"
#include "ch_app_context_tcp.h"

ch_tcp_work_t *
ch_tcp_work_create(ch_pool_t *mp,const char *cfname){


	ch_tcp_work_t *tcp_work = NULL;

	ch_tcp_context_t *tcp_context = NULL;

	tcp_work = (ch_tcp_work_t*)ch_palloc(mp,sizeof(*tcp_work));

	tcp_work->mp = mp;

	tcp_context = ch_tcp_context_create(mp,cfname);

	if(tcp_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create TCP Context for tcp work!");
		return NULL;
	}

	tcp_work->tcp_context = tcp_context;

	tcp_work->pint_context = ch_process_interface_tcp_context_create(tcp_work->mp,
		tcp_context->pint_cfname,0);

	if(tcp_work->pint_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create process interface context for reading!");
		return NULL;

	}

	tcp_work->app_context = ch_app_context_tcp_create(tcp_work->mp,tcp_context->tcp_app_cfname);
	if(tcp_work->app_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create tcp application context:%s",tcp_context->tcp_app_cfname);
		return NULL;
	}

	tcp_work->tstpool = ch_tcp_session_task_pool_create(tcp_work);
	if(tcp_work->tstpool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create tcp session task pool!");
		return NULL;
	}

	return tcp_work;
}

int ch_tcp_work_start(ch_tcp_work_t *tcp_work){

	/*start all tcp session tasks*/
	return ch_tcp_session_task_pool_start(tcp_work->tstpool);

}

void ch_tcp_work_stop(ch_tcp_work_t *tcp_work){

	ch_tcp_session_task_pool_stop(tcp_work->tstpool);

}
