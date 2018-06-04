/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_pool_tcp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月29日 11时19分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_constants.h"
#include "ch_log.h"
#include "ch_util.h"
#include "ch_assemble_pool_tcp.h"
#include "ch_assemble_packet_tcp.h"
#include "ch_assemble_tcp.h"

static int _assemble_session_find(ch_assemble_pool_tcp_t *aspt,ch_assemble_session_tcp_t **asst,ch_assemble_task_t **astask,
        ch_assemble_packet_t *as_pkt){

    unsigned int i,n;
    ch_assemble_tcp_t *assemble_tcp;

    ch_assemble_task_t **astsks,*astsk;
    ch_assemble_session_tcp_t *res = NULL;
    apr_array_header_t *astsk_arr = aspt->asp.assemble_tasks; 
    *asst = NULL;
    *astask = NULL;

    n = astsk_arr->nelts;
    astsks = (ch_assemble_task_t**)(astsk_arr->elts);
   
    for(i = 0; i<n;i++){

        astsk = astsks[i];
        assemble_tcp = (ch_assemble_tcp_t*)astsk->as;
        res = ch_assemble_session_pool_tcp_entry_find(assemble_tcp->asspt,as_pkt);
        if(res){
            /*Found*/
            *asst = res;
            *astask = astsk;
            return 0;
        }
    }

    /*no found!*/
    return -1;
} 

static inline int _is_tcp_ack_ack_packet(ch_tcp_session_request_t *sreq,ch_assemble_packet_tcp_t *tas_pkt){

    uint32_t recv_ack = tas_pkt->recv_ack;

    return (tas_pkt->tcp_flags&CH_TH_ACK)&&(sreq->three_way_state == THREE_WAY_SYN_ACK)&&(recv_ack == sreq->res_sn_init);
}

static inline int _is_tcp_data_packet(ch_assemble_packet_tcp_t *tas_pkt){

    return tas_pkt->payload_len>0;
}

static inline int _is_need_ignore(ch_assemble_session_tcp_t *asst,ch_assemble_packet_tcp_t *tas_pkt){

    if(_is_tcp_data_packet(tas_pkt))
        return 0;

    if(ch_tcp_session_state_is_fin(asst)||ch_tcp_session_state_is_rst(asst))
        return 0;

    return 1;
}

static inline void _set_assemble_state(ch_assemble_session_tcp_t *asst,ch_assemble_packet_tcp_t *tas_pkt){

    if(ch_tcp_session_state_is_fin(asst)||ch_tcp_session_state_is_rst(asst)){
        return;
    }

    if(_is_tcp_data_packet(tas_pkt)){
        ch_assemble_session_tcp_state_set(asst,SESSION_STATE_DATA);
    }else if(is_tcp_fin_packet(tas_pkt)){
        ch_assemble_session_tcp_state_set(asst,SESSION_STATE_FIN);
    }else if(is_tcp_rst_packet(tas_pkt)){
        ch_assemble_session_tcp_state_set(asst,SESSION_STATE_RST);
    }else{
        /*do nothing*/
    }
}

static inline void _update_assemble_last_ack(ch_assemble_session_tcp_t *asst,ch_assemble_packet_tcp_t *tas_pkt){

	ch_assemble_session_endpoint_t *ep = ch_assemble_session_tcp_pair_endpoint_get(asst,(ch_assemble_packet_t*)tas_pkt);
	if(ep){
	
		ep->last_ack = CH_MAX(ep->last_ack,
			ch_assemble_session_endpoint_offset_get(ep,tas_pkt->recv_ack));
	}
}

static inline int _need_update_last_ack(ch_assemble_session_tcp_t *asst,ch_assemble_packet_tcp_t *tas_pkt){

	if(ch_tcp_session_state_is_fin(asst)||ch_tcp_session_state_is_rst(asst))
		return 0;
	
	if(is_tcp_fin_packet(tas_pkt)||is_tcp_rst_packet(tas_pkt))
		return 0;

	return 1;
}

static int _tcp_data_assemble_process(ch_assemble_task_t *astask,ch_assemble_session_tcp_t *asst,ch_assemble_packet_tcp_t *tas_pkt){

    _set_assemble_state(asst,tas_pkt);
	
	if(_need_update_last_ack(asst,tas_pkt))
		_update_assemble_last_ack(asst,tas_pkt);

    if(_is_need_ignore(asst,tas_pkt)){

        return -1;
    }

    /*put this packet into assemble tasks and waiting to process it!*/
    if(ch_assemble_task_pkt_put(astask,(ch_assemble_packet_t*)tas_pkt)){
        /*failed ,so need to update assemble session's need_end == 1,*/
        asst->need_end = 1;
        return -1;
    }

    ch_assemble_task_stats_update(astask,1,0);

    return 0;
}

static int _assemble_session_create(ch_assemble_pool_tcp_t *aspt,
        ch_tcp_session_request_t *sreq,ch_assemble_packet_tcp_t *tas_pkt,ch_app_t *app){

    ch_assemble_task_t *astask;
    ch_assemble_session_tcp_t *asst;
    ch_assemble_tcp_t *tas;

    astask = ch_assemble_pool_task_schedule((ch_assemble_pool_t*)aspt);
    if(astask == NULL){
        ch_log(CH_LOG_ERR,"Cannot get an assemble task to handle this packets,so drop it!");
        return -1;
    }

    tas = (ch_assemble_tcp_t*)astask->as;
    asst = ch_assemble_session_pool_tcp_entry_create(tas->asspt,sreq,app,(ch_assemble_packet_t*)tas_pkt);
    
    if(asst == NULL){
        ch_log(CH_LOG_ERR,"Create an assemble session failed,so drop this packet!");
        return -1;
    }

    asst->session_id = ch_assemble_id_get((ch_assemble_t*)tas);

    ch_assemble_task_stats_update(astask,0,1);
    /*ok,now handle this data packet,*/
    if(_tcp_data_assemble_process(astask,asst,tas_pkt)){
        return -1;
    }

    return 0;
}

static int _three_way_handshake_process(ch_assemble_pool_tcp_t *aspt,ch_tcp_session_request_t *sreq,
        ch_assemble_packet_tcp_t *tas_pkt,ch_app_t *app){

    ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)tas_pkt;
    int rc = PROCESSOR_RET_DROP;
	int ret;

    uint32_t sent_seq = tas_pkt->sent_seq;
    uint32_t src_ip = as_pkt->src_ip;
    uint32_t dst_ip = as_pkt->dst_ip;
    uint16_t src_port = as_pkt->src_port;
    uint16_t dst_port = as_pkt->dst_port;

#if 0
    char srcip[32] = {0};
    char dstip[32] = {0};

	ch_log(CH_LOG_ERR,"handshake: %s:%u<-->%s:%u, flags: %0x.\n", 
		ch_ipaddr_str_get(srcip, 32, src_ip), 
		src_port, 
		ch_ipaddr_str_get(dstip, 32, dst_ip), 
		dst_port, tas_pkt->tcp_flags);
#endif
	
    if(sreq == NULL){

        if(is_tcp_syn_packet(tas_pkt)){

            /*create a new tcp session request!*/
            sreq = ch_tcp_session_request_create(aspt->tsr_pool,as_pkt);
            if(sreq == NULL){
                ch_log(CH_LOG_ERR,"create tcp session request failed!");
                return PROCESSOR_RET_DROP;
            }
            /*init session request*/
            sreq->req_sn_init = sent_seq+1;
            sreq->req_ip = src_ip;
            sreq->req_port = src_port;
            sreq->res_ip = dst_ip;
            sreq->res_port = dst_port;
            sreq->three_way_state = THREE_WAY_SYN;
            /*drop this packet!*/
        }
    }else{

        if(is_tcp_syn_packet(tas_pkt)){
            /*this syn packet has been retransmited,*/
        }else if(is_tcp_syn_ack_packet(tas_pkt)){
            if(sreq->three_way_state == THREE_WAY_SYN){
                sreq->res_sn_init = sent_seq+1;
                sreq->three_way_state = THREE_WAY_SYN_ACK;
            }else{
                /*drop it */
				//ch_log(CH_LOG_WARN,"syn drop, retransmitted:\n");
                //return -1;
            }
			
        }else if(_is_tcp_ack_ack_packet(sreq,tas_pkt)){

            if(sreq->three_way_state == THREE_WAY_SYN_ACK){
                /**/
				//ch_log(CH_LOG_WARN,"3 handshake ack.\n");
                sreq->three_way_state = THREE_WAY_ACK_ACK;
            }else{
				//ch_log(CH_LOG_WARN,"ack ack drop, retransmitted:\n");
                /*drop this packet,maybe retransmitted*/
            }

        }else if(_is_tcp_data_packet(tas_pkt)){

            if(sreq->three_way_state == THREE_WAY_ACK_ACK){
                
                /*create a new assemble session
                 *1: get an assemble task
                 *2: create an assemble session
                 *3: free session request instance back into pool
                 * */
                ret = _assemble_session_create(aspt,sreq,tas_pkt,app);
                ch_tcp_session_request_free(aspt->tsr_pool,sreq);

                if(ret == 0){
                    rc = PROCESSOR_RET_OK;
                }
            }else{
                /*drop this packet*/
				//ch_log(CH_LOG_WARN,"111 drop:\n");
            }

        }else{
            /*other packet(ack,fin),ignore and drop this packet!*/
			//ch_log(CH_LOG_WARN,"other packet, ignore and drop:\n");
        }
    }

    /*ok*/
    return rc;
}


int tcp_packet_process(ch_assemble_pool_t *asp,ch_assemble_packet_t *as_pkt){

	char b[128];
	size_t c = 0;
    int rc,res = PROCESSOR_RET_OK;
    ch_assemble_pool_tcp_t *aspt = (ch_assemble_pool_tcp_t*)asp;
    ch_tcp_session_request_t *sreq = NULL;
    ch_assemble_session_tcp_t *asst = NULL;
    ch_assemble_task_t *astask = NULL;
    ch_assemble_packet_tcp_t *tas_pkt = (ch_assemble_packet_tcp_t*)as_pkt;
    ch_app_t *app;

	do{
		app = ch_app_context_recognize_by_port((ch_app_context_t*)aspt->app_tcp_context,as_pkt->src_port,as_pkt->dst_port); 


		if(app == NULL){
			/*no application handles this packet!,skip it!*/
			res = PROCESSOR_RET_DROP;
			//ch_log(CH_LOG_WARN,"no application handles this packaet, drop:%lu\n %s",(unsigned long)c,b);
			break;
		}

		sreq = ch_tcp_session_request_find(aspt->tsr_pool,as_pkt);
		rc = _assemble_session_find(aspt,&asst,&astask,as_pkt);

		if(rc == 0){

			/*can be assembled*/
			if(_tcp_data_assemble_process(astask,asst,tas_pkt)){
				/*process assemble failed,drop this packet!*/
				//ch_log(CH_LOG_DEBUG,"assemble failed, drop:%lu\n %s",(unsigned long)c,b);
				res = PROCESSOR_RET_DROP;
				break;
			}

		}else{
			/*handle three way handshake!*/
			if(_three_way_handshake_process(aspt,sreq,tas_pkt,app)==PROCESSOR_RET_DROP){
				/*some error ,drop this packet!*/
				res = PROCESSOR_RET_DROP;
				//ch_log(CH_LOG_DEBUG,"tcp handshake got error, drop:%lu\n %s",(unsigned long)c,b);
				break;
			}
		}
	}while(0);

	c = ch_ptable_entries_timeout_free(aspt->tsr_pool->as_tbl,
		NULL);

	if(c){

		fprintf(stdout,"Clean tcp request session:%lu\n",(unsigned long)c);
		ch_ptable_dump(aspt->tsr_pool->as_tbl,stdout);

	}

    return res;
}

static int tcp_assemble_tasks_create(ch_assemble_pool_tcp_t *aspt,size_t assemble_tasks_n){

    size_t i ;
    ch_task_t *tk;
    ch_assemble_t *as;

    for(i = 0; i<assemble_tasks_n; i++){

        as = ch_assemble_tcp_create(aspt,i+1);
        if(as == NULL){
        
            ch_log(CH_LOG_ERR,"Create assemble failed for assemble task!");

            return -1;
        }

        tk = ch_assemble_task_create(aspt->tcp_context.context,as,"tcp_assemble_task",i+1);
        if(tk == NULL){
        
            ch_log(CH_LOG_ERR,"Create assemble task[%d] failed!",i+1);
            return -1;
        }

        ch_assemble_pool_task_put((ch_assemble_pool_t*)aspt,tk);

    }

    return 0;
}

static void tcp_assemble_tasks_destroy(ch_assemble_pool_t *asp){

    size_t i ;
    size_t assemble_tasks_n ;

    ch_assemble_task_t *tk,**tks;
    tks = (ch_assemble_task_t**)asp->assemble_tasks->elts;
    assemble_tasks_n = asp->assemble_tasks->nelts;

    for(i = 0; i<assemble_tasks_n; i++){

        tk = tks[i];
        ch_assemble_tcp_destroy((ch_assemble_tcp_t *)tk->as);
    }

}

ch_assemble_pool_t * ch_assemble_pool_tcp_create(ch_context_t *context){
    
    ch_assemble_pool_tcp_t *aspt = NULL;
    size_t assemble_tasks_n ;

    aspt = (ch_assemble_pool_tcp_t*)apr_palloc(context->mp,sizeof(ch_assemble_pool_tcp_t));

    if(-1 == ch_assemble_context_tcp_init(context,&aspt->tcp_context)){

        return NULL;
    }

    assemble_tasks_n = aspt->tcp_context.assemble_tasks_n;
    ch_assemble_pool_init(context,&aspt->asp,0,assemble_tasks_n,aspt->tcp_context.mmap_file_dir,
            tcp_packet_process);

    aspt->tsr_pool = ch_tcp_session_request_pool_create(&aspt->tcp_context); 

    if(aspt->tsr_pool == NULL){
    
        return NULL;
    }
    
    aspt->app_tcp_context = ch_app_context_tcp_create(context);
    if(aspt->app_tcp_context == NULL){
    
        return NULL;
    }
    
    tcp_assemble_tasks_create(aspt,assemble_tasks_n);

    return (ch_assemble_pool_t*)aspt;
}

void ch_assemble_pool_tcp_destroy(ch_assemble_pool_t *asp){

    ch_assemble_pool_tcp_t *aspt = (ch_assemble_pool_tcp_t*)asp;

    tcp_assemble_tasks_destroy(asp);
    ch_tcp_session_request_pool_destroy(aspt->tsr_pool);
}
