/*
 *
 *      Filename: ch_sa_session_task.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-30 11:49:48
 * Last Modified: 2018-08-01 12:02:12
 */

#include "ch_sa_session_task.h"
#include "ch_sa_packet_dp.h"
#include "ch_log.h"
#include "ch_packet_record.h"

static inline const char * _get_name(ch_pool_t *mp,const char *prefix,uint32_t tsk_id){

	return ch_psprintf(mp,"%s/sa_%lu",prefix,(unsigned long)tsk_id);
}

static int _sa_session_task_run(ch_task_t *task,void *priv_data ch_unused){

	ch_sa_session_task_t *sa_task = (ch_sa_session_task_t*)task;
	ch_packet_t *pkt;


	pkt = ch_process_queue_pop(sa_task->pqueue);

	if(pkt){
	
		ch_sa_packet_dp(sa_task,pkt);

		ch_packet_free(pkt);

	}

	return TASK_RETURN_OK;
}

static int _sa_session_task_init(ch_task_t *task ch_unused,void *priv_data ch_unused){


	return 0;
}

static int _sa_session_task_exit(ch_task_t *task ch_unused,void *priv_data ch_unused){


	return 0;

}

ch_sa_session_task_t * ch_sa_session_task_create(ch_sa_work_t *sa_work,uint32_t task_id){

	ch_sa_context_t *sa_context = sa_work->sa_context;

	ch_sa_session_task_t *sa_session_task = NULL;

	sa_session_task = (ch_sa_session_task_t*)ch_palloc(sa_work->mp,sizeof(*sa_session_task));

	sa_session_task->task.tsk_id = task_id;
	sa_session_task->task.run = _sa_session_task_run;
	sa_session_task->task.init = _sa_session_task_init;
	sa_session_task->task.exit = _sa_session_task_exit;

	sa_session_task->sa_work = sa_work;

	sa_session_task->pqueue = ch_process_interface_queue_assign(sa_work->pint_sa_context->pint);
	
	if(sa_session_task->pqueue == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot assign a process queue for this sa session task:%d",(int)task_id);
		return NULL;
	}

	sa_session_task->tcp_req_handler = ch_sa_tcp_session_request_handler_create(sa_work,sa_session_task);
	if(sa_session_task->tcp_req_handler == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create sa session request handler for this sa session task:%d",(int)task_id);

		return NULL;
	}

	sa_session_task->tcp_session_handler = ch_sa_tcp_session_handler_create(sa_work,sa_session_task);
	if(sa_session_task->tcp_session_handler == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create sa session handler for this sa session task:%d",(int)task_id);

		return NULL;
	}
	
	sa_session_task->udp_handler = ch_sa_udp_session_handler_create(sa_work,sa_session_task);
	if(sa_session_task->udp_handler == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create udp session handler for this sa session task:%d",(int)task_id);

		return NULL;
	}


	if(sa_context->mmap_file_dir){
	
		sa_session_task->shm_fmt = ch_shm_format_pkt_with_mmap_create(sa_work->mp,
			_get_name(sa_work->mp,sa_context->mmap_file_dir,sa_session_task->task.tsk_id),
			sa_context->shm_size,
			sa_context->entry_size,0,1);

	}else if(sa_context->key){
		
		sa_session_task->shm_fmt = ch_shm_format_pkt_with_shm_create(sa_work->mp,
			_get_name(sa_work->mp,sa_context->key,sa_session_task->task.tsk_id),
			sa_context->proj_id,
			sa_context->shm_size,
			sa_context->entry_size,0,1);
	
	}
	
	if(sa_session_task->shm_fmt == NULL){
	
		ch_log(CH_LOG_ERR,"Create sa session task failed,cannot create shm format!");
		return NULL;
	}

	if(ch_buffer_init(&sa_session_task->sa_buffer))
	{
	
		ch_log(CH_LOG_ERR,"Create buffer failed for sa session!");
		return NULL;
	}

	sa_session_task->dstore_pool = ch_sa_data_store_pool_create(sa_context->payload_data_size,sa_context->dstore_limits);
	if(sa_session_task->dstore_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create data store pool failed!");
		return NULL;
	}

#if 0
    sa_session_task->lua_engine = NULL;

    if(sa_context->lua_fname){
        
        sa_session_task->lua_engine = ch_lua_engine_create(sa_work->mp,
                sa_context->lua_path,"",
                "sa_stat_init",
                "sa_stat_run",
                "sa_stat_fin",
                0,
                sa_context->lua_fname,
                "__sa_stat_shm_fmt",
                sa_session_task->shm_fmt); 
        
        if(sa_session_task->lua_engine == NULL){
        
            ch_log(CH_LOG_ERR,"Create lua engine failed for SA!");
            return NULL;
        }

    }
#endif

	return sa_session_task;
}

