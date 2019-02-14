/*
 *
 *      Filename: ch_packet_rxtask_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-08 15:16:22
 * Last Modified: 2018-02-06 15:34:32
 */

#include "ch_log.h"
#include "ch_constants.h"
#include "ch_task.h"
#include "ch_packet_rxtask_pool.h"
#include "ch_packet_rxtask.h"

static int _get_rxtasks_number(ch_pdispatcher_context_t *pdcontext){

	int config_rxtasks,cpu_cores,port_queues,rxtasks=1;

	config_rxtasks = (int)(pdcontext->packet_rxtasks_n);
	cpu_cores = (int)(pdcontext->cpool->config_core_count);
	port_queues = (int)(pdcontext->ppool->port_queue_n);

	rxtasks = CH_MIN(config_rxtasks,cpu_cores);
	rxtasks = CH_MIN(port_queues,rxtasks);

	return rxtasks;
}

static int _create_all_rxtasks(ch_pdispatcher_context_t *pdcontext,ch_packet_rxtask_pool_t *rxtask_pool){

	ch_task_t *task;
	int rxtasks_n = 0,per_port_queue_n,max_port_queue_n,port_queue_n,task_i,queue_n;
	ch_port_pool_t *ppool = pdcontext->ppool;

	port_queue_n = ppool->port_queue_n;

	rxtasks_n = _get_rxtasks_number(pdcontext);
	
	per_port_queue_n = port_queue_n/rxtasks_n;
	max_port_queue_n = per_port_queue_n+port_queue_n%rxtasks_n;

	rxtask_pool->rxtasks_n = rxtasks_n;
	rxtask_pool->per_port_queue_n = per_port_queue_n;
	rxtask_pool->max_port_queue_n = max_port_queue_n;

	for(task_i = 0; task_i<rxtasks_n;task_i++){
	
		if((task_i == rxtasks_n-1)&&(per_port_queue_n!=max_port_queue_n)){
		
			queue_n = max_port_queue_n;
		}else{
		
			queue_n = per_port_queue_n;
		}
		
		task = ch_packet_rxtask_create(pdcontext,task_i+1,queue_n);

		if(task == NULL){
		
			ch_log(CH_LOG_ERR,"Create packet receive task:%d failed!",task_i+1);
			return 0;
		}
		
		ch_log(CH_LOG_INFO,"Create packet receive task:%d,bind to cpu core:%d,port queues:%d",
			task_i+1,(int)task->core->core_id,queue_n);

		/*add task into pool*/
		*(ch_task_t**)ch_array_push(rxtask_pool->rxtasks) = task;
	}

	/*ok*/
	return rxtasks_n;
}

ch_packet_rxtask_pool_t * ch_packet_rxtask_pool_create(ch_pdispatcher_context_t *pdcontext){

	ch_packet_rxtask_pool_t *rxtask_pool = NULL;

	rxtask_pool = (ch_packet_rxtask_pool_t*)ch_pcalloc(pdcontext->mp,sizeof(*rxtask_pool));

	rxtask_pool->pdcontext = pdcontext;
	rxtask_pool->rxtasks = ch_array_make(pdcontext->mp,16,sizeof(ch_task_t*));
	
	rxtask_pool->rxtasks_n = 0;
	rxtask_pool->per_port_queue_n = 0;
	rxtask_pool->max_port_queue_n = 0;

	if(_create_all_rxtasks(pdcontext,rxtask_pool)== 0){
	
		/*failed*/
		ch_log(CH_LOG_ERR,"Create packet receive tasks failed!");
		return NULL;
	}

	ch_log(CH_LOG_INFO,"Create packet receive tasks:%d ok!",rxtask_pool->rxtasks_n);

	/*ok*/
	return rxtask_pool;
}
