/*
 *
 *      Filename: ch_udp_work.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-10 16:27:54
 * Last Modified: 2018-07-16 12:24:18
 */
#include "ch_udp_work.h"
#include "ch_udp_context.h"
#include "ch_log.h"

ch_udp_work_t * ch_udp_work_create(ch_pool_t *mp,const char *cfname){

	ch_udp_work_t *udp_work = NULL;

	ch_udp_context_t *udp_context = NULL;

	udp_work = (ch_udp_work_t*)ch_palloc(mp,sizeof(*udp_work));

	udp_work->mp = mp;

	udp_context = ch_udp_context_create(mp,cfname);

	if(udp_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create udp Context for udp work!");
		return NULL;
	}

	udp_work->udp_context = udp_context;

    /*init log*/
    ch_log_init(mp,udp_context->log_name,udp_context->log_level);
	
	udp_work->pint_udp_context = ch_process_interface_udp_context_create(udp_work->mp,
		udp_context->pint_udp_cfname,0);

	if(udp_work->pint_udp_context == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create udp process interface context for reading!");
		return NULL;

	}

	udp_work->udpt_pool = ch_udp_session_task_pool_create(udp_work);
	if(udp_work->udpt_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create udp session task pool!");
		return NULL;
	}

	udp_work->app_pool = ch_udp_app_pool_create(mp,udp_context->app_udp_cfname); 
	if(udp_work->app_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create udp app pool!");
		return NULL;
	}

	return udp_work;

}

int ch_udp_work_start(ch_udp_work_t *udp_work){

	/*start all udp session tasks*/
	return ch_udp_session_task_pool_start(udp_work->udpt_pool);

}

void ch_udp_work_stop(ch_udp_work_t *udp_work){

	ch_udp_session_task_pool_stop(udp_work->udpt_pool);

}

