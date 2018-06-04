/*
 *
 *      Filename: ch_assemble_pool_udp.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-14 15:24:26
 * Last Modified: 2017-06-13 16:16:31
 */

#include <rte_ip_frag.h>
#include "ch_assemble_pool_udp.h"
#include "ch_constants.h"
#include "ch_log.h"
#include "ch_assemble_packet_udp.h"
#include "ch_assemble_udp.h"
#include "ch_app_context_udp.h"

static inline ch_assemble_task_t * _astask_get(ch_assemble_pool_t *asp){

	ch_assemble_task_t **arr = (ch_assemble_task_t**)asp->assemble_tasks->elts;

	return arr[0];
}

int udp_packet_process(ch_assemble_pool_t *asp,ch_assemble_packet_t *as_pkt){

    ch_assemble_pool_udp_t *aspu = (ch_assemble_pool_udp_t*)asp;
    ch_assemble_task_t *astask = NULL;
    ch_app_t *app;
	struct ipv4_hdr *iph = ch_ipv4_hdr_get(as_pkt->mbuf);
	struct udp_hdr *uh;

	if(rte_ipv4_frag_pkt_is_fragmented(iph)==0){
		
		uh = ch_udp_hdr_get(as_pkt->mbuf);
    
		as_pkt->src_port = rte_be_to_cpu_16(uh->src_port);
    
		as_pkt->dst_port = rte_be_to_cpu_16(uh->dst_port);

#if 0
		app = ch_app_context_recognize_by_port((ch_app_context_t*)aspu->app_udp_context,as_pkt->src_port,as_pkt->dst_port); 


		if(app == NULL){
			/*no application handles this packet!,skip it!*/
			return PROCESSOR_RET_DROP;
		}
#endif

	}

	/* add to ring  */
	astask = _astask_get(asp);

    if(ch_assemble_task_pkt_put(astask,as_pkt)){
	
		return PROCESSOR_RET_DROP;
	}

    return PROCESSOR_RET_OK;
}

static int udp_assemble_tasks_create(ch_assemble_pool_udp_t *aspd,size_t assemble_tasks_n){

    size_t i ;
    ch_task_t *tk;
    ch_assemble_t *as;

    for(i = 0; i<assemble_tasks_n; i++){

        as = ch_assemble_udp_create(aspd,(int)(i+1));
        if(as == NULL){
        
            ch_log(CH_LOG_ERR,"Create assemble failed for assemble task!");

            return -1;
        }

        tk = ch_assemble_task_create(aspd->udp_context.context,as,"udp_assemble_task",i+1);
        if(tk == NULL){
        
            ch_log(CH_LOG_ERR,"Create assemble task[%d] failed!",i+1);
            return -1;
        }

        ch_assemble_pool_task_put((ch_assemble_pool_t*)aspd,tk);

    }

    return 0;
}

ch_assemble_pool_t * ch_assemble_pool_udp_create(ch_context_t *context){
    
    ch_assemble_pool_udp_t *aspd = NULL;

    aspd = (ch_assemble_pool_udp_t*)apr_palloc(context->mp,sizeof(ch_assemble_pool_udp_t));

    if(-1 == ch_assemble_context_udp_init(context,&aspd->udp_context)){

        return NULL;
    }

    ch_assemble_pool_init(context,&aspd->asp,0,1,aspd->udp_context.mmap_file_dir,
            udp_packet_process);

    aspd->app_udp_context = ch_app_context_udp_create(context);
    if(aspd->app_udp_context == NULL){
    
        return NULL;
    }
    
    udp_assemble_tasks_create(aspd,1);

    return (ch_assemble_pool_t*)aspd;
}
