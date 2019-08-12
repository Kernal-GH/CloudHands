/*
 *
 *      Filename: ch_udp_session_task.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-30 11:49:48
 * Last Modified: 2018-09-13 16:04:38
 */

#include "ch_udp_session_task.h"
#include "ch_log.h"
#include "ch_packet_record.h"
#include "ch_udp_session_handler.h"
#include "ch_udp_session_request_handler.h"

static inline const char * _get_name(ch_pool_t *mp,const char *prefix,uint32_t tsk_id){

	return ch_psprintf(mp,"%s/udp_%lu",prefix,(unsigned long)tsk_id);
}

static int _udp_session_task_run(ch_task_t *task,void *priv_data ch_unused){

	ch_udp_session_task_t *udp_task = (ch_udp_session_task_t*)task;
	ch_packet_t *pkt;

	pkt = ch_process_queue_pop(udp_task->pqueue);

	if(pkt){
	
		ch_udp_session_request_packet_handle(udp_task->udp_session_req_handler,pkt);

		ch_packet_free(pkt);

	}

	ch_udp_session_task_handlers_timeout_free(udp_task);

	return TASK_RETURN_OK;
}

static int _udp_session_task_init(ch_task_t *task ch_unused,void *priv_data ch_unused){


	return 0;
}

static int _udp_session_task_exit(ch_task_t *task ch_unused,void *priv_data ch_unused){


	return 0;

}

ch_udp_session_task_t * ch_udp_session_task_create(ch_udp_work_t *udp_work,uint32_t task_id){

	ch_udp_context_t *udp_context = udp_work->udp_context;

	ch_udp_session_task_t *udp_session_task = NULL;

	udp_session_task = (ch_udp_session_task_t*)ch_palloc(udp_work->mp,sizeof(*udp_session_task));

	udp_session_task->task.tsk_id = task_id;
	udp_session_task->task.run = _udp_session_task_run;
	udp_session_task->task.init = _udp_session_task_init;
	udp_session_task->task.exit = _udp_session_task_exit;

	udp_session_task->udp_work = udp_work;

	udp_session_task->pqueue = ch_process_interface_queue_assign(udp_work->pint_udp_context->pint);
	
	if(udp_session_task->pqueue == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot assign a process queue for this udp session task:%d",(int)task_id);
		return NULL;
	}

	udp_session_task->udp_session_handler = ch_udp_session_handler_create(udp_work,udp_session_task);
	if(udp_session_task->udp_session_handler == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create udp session handler for this udp session task:%d",(int)task_id);

		return NULL;
	}
	
	udp_session_task->udp_session_req_handler = ch_udp_session_request_handler_create(udp_work,udp_session_task);
	if(udp_session_task->udp_session_req_handler == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create udp session request handler for this udp session task:%d",(int)task_id);

		return NULL;
	}
	
	if(udp_context->mmap_file_dir){
	
		udp_session_task->shm_fmt = ch_shm_format_pkt_with_mmap_create(udp_work->mp,
			_get_name(udp_work->mp,udp_context->mmap_file_dir,udp_session_task->task.tsk_id),
			udp_context->shm_size,
			udp_context->entry_size,0,1);

	}else if(udp_context->key){
		
		udp_session_task->shm_fmt = ch_shm_format_pkt_with_shm_create(udp_work->mp,
			_get_name(udp_work->mp,udp_context->key,udp_session_task->task.tsk_id),
			udp_context->proj_id,
			udp_context->shm_size,
			udp_context->entry_size,0,1);
	
	}
	
	if(udp_session_task->shm_fmt == NULL){
	
		ch_log(CH_LOG_ERR,"Create udp session task failed,cannot create shm format!");
		return NULL;
	}

    if(udp_context->use_msgpack){

        udp_session_task->dstore = ch_msgpack_store_create(udp_work->mp);
        if(udp_session_task->dstore == NULL){
            ch_log(CH_LOG_ERR,"Create udp session task failed,cannot create msgpack store!");
            return NULL;
        }

    }else{ 
        if(ch_dout_init(&udp_session_task->dout))
        {
        
            ch_log(CH_LOG_ERR,"Init data output failed for udp session!");
            return NULL;
        }
    }
	return udp_session_task;
}

