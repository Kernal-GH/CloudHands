/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月28日 15时21分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_constants.h"
#include "ch_assemble_pool.h"
#include "ch_black_white_list.h"
static inline int _need_update_assemble_task(ch_assemble_task_t *pre,ch_assemble_task_t *cur){

    if(cur->stats.sessions < pre->stats.sessions)
        return 1;

    return 0;
}

ch_assemble_task_t * ch_assemble_pool_task_schedule(ch_assemble_pool_t *asp){

    unsigned int i,n;
    ch_assemble_task_t *ask = NULL,*res = NULL;
    ch_assemble_task_t **asks = (ch_assemble_task_t **)asp->assemble_tasks->elts;

    n = asp->assemble_tasks->nelts;

    for( i = 0; i<n; i++){

        ask = asks[i];

        if(res == NULL){
            res = ask;
            continue;
        }

        if(_need_update_assemble_task(res,ask)){

            res = ask;
        }
    }

    return res;
}

void ch_assemble_pool_init(ch_context_t *context,ch_assemble_pool_t *asp,int asp_id,size_t assemble_tasks_n,
        const char *fdir,
        int (*packet_process)(ch_assemble_pool_t *asp,ch_assemble_packet_t *as_pkt)){


    rte_spinlock_init(&asp->spin_lock);
    asp->context = context;
    asp->assemble_tasks = apr_array_make(context->mp,assemble_tasks_n,sizeof(ch_assemble_task_t*));

    asp->asp_id = asp_id;
    asp->assemble_tasks_n = assemble_tasks_n;
    asp->mmap_file_dir = fdir;
    asp->packet_process = packet_process;
}

int ch_assemble_pool_packet_process(ch_assemble_pool_t *asp,ch_assemble_packet_t *as_pkt){

	int rc;

	ch_assemble_pool_lock(asp);

	if(ch_black_white_list_match(asp->context->bwl,as_pkt->src_ip,as_pkt->src_port)||\
		ch_black_white_list_match(asp->context->bwl,as_pkt->dst_ip,as_pkt->dst_port))
	{
	
		rc = asp->packet_process(asp,as_pkt);
	}else{
	
		/*not match ignore this packets*/
		rc = PROCESSOR_RET_DROP; 
	}

	ch_assemble_pool_unlock(asp);

	return rc;
}


