/*
 *
 *      Filename: ch_tcp_session_task.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-02 11:30:21
 * Last Modified: 2018-08-01 11:57:37
 */

#include "ch_tcp_session_task.h"
#include "ch_log.h"
#include "ch_packet.h"
#include "ch_packet_tcp.h"
#include <rte_mbuf.h>

static int _tcp_session_task_run(ch_task_t *task,void *priv_data ch_unused){

	ch_packet_tcp_t tcp_pkt;

	ch_tcp_session_task_t *tcp_task = (ch_tcp_session_task_t*)task;
	ch_packet_t *pkt;

	pkt = ch_process_queue_pop(tcp_task->pqueue);

	if(pkt){
	
		ch_packet_tcp_init_from_pkt(&tcp_pkt,pkt);

		ch_tcp_session_request_packet_handle(tcp_task->sreq_handler,&tcp_pkt);

		ch_packet_free(pkt);

	}

	return TASK_RETURN_OK;
}

static int _tcp_session_task_init(ch_task_t *task ch_unused,void *priv_data ch_unused){


	return 0;
}

static int _tcp_session_task_exit(ch_task_t *task ch_unused,void *priv_data ch_unused){


	return 0;

}

ch_tcp_session_task_t * ch_tcp_session_task_create(ch_tcp_work_t *tcp_work,uint32_t task_id){


	ch_tcp_session_task_t *tcp_session_task = NULL;

	tcp_session_task = (ch_tcp_session_task_t*)ch_palloc(tcp_work->mp,sizeof(*tcp_session_task));

	tcp_session_task->task.tsk_id = task_id;
	tcp_session_task->task.run = _tcp_session_task_run;
	tcp_session_task->task.init = _tcp_session_task_init;
	tcp_session_task->task.exit = _tcp_session_task_exit;

	tcp_session_task->tcp_work = tcp_work;

	tcp_session_task->pqueue = ch_process_interface_queue_assign(tcp_work->pint_context->pint);
	
	if(tcp_session_task->pqueue == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot assign a process queue for this tcp session task:%d",(int)task_id);
		return NULL;
	}

	tcp_session_task->sreq_handler = ch_tcp_session_request_handler_create(tcp_work,tcp_session_task);
	if(tcp_session_task->sreq_handler == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create tcp session request handler for this tcp session task:%d",(int)task_id);

		return NULL;
	}

	tcp_session_task->shandler = ch_tcp_session_handler_create(tcp_work,tcp_session_task);
	if(tcp_session_task->shandler == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create tcp session handler for this tcp session task:%d",(int)task_id);

		return NULL;
	}

	return tcp_session_task;
}

