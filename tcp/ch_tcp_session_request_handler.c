/*
 *
 *      Filename: ch_tcp_session_request_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-02 11:44:08
 * Last Modified: 2018-09-25 15:55:51
 */

#include "ch_tcp_session_request_handler.h"
#include "ch_log.h"
#include "ch_atomic.h"
#include "ch_tcp_session.h"
#include "ch_tcp_session_pool.h"

static ch_atomic64_t cur_session_id,*cur_session_id_ptr=&cur_session_id;

ch_tcp_session_request_handler_t *
ch_tcp_session_request_handler_create(ch_tcp_work_t *tcp_work,ch_tcp_session_task_t *session_task){


	ch_tcp_session_request_handler_t *req_handler = NULL;

	req_handler = (ch_tcp_session_request_handler_t*)ch_palloc(tcp_work->mp,sizeof(*req_handler));

	req_handler->session_task = session_task;
	req_handler->tcp_work = tcp_work;

	req_handler->req_pool = ch_tcp_session_request_pool_create(tcp_work->tcp_context,0,NULL,NULL);

	if(req_handler->req_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create tcp session request pool for handler!");
		return NULL;
	}

	ch_atomic64_init(cur_session_id_ptr);

	return req_handler;
}

static inline int _is_tcp_ack_ack_packet(ch_tcp_session_request_t *sreq,ch_packet_tcp_t *tcp_pkt){

    uint32_t recv_ack = tcp_pkt->recv_ack;
    return (tcp_pkt->tcp_flags&CH_TH_ACK)&&(sreq->three_way_state == THREE_WAY_SYN_ACK)&&(recv_ack == sreq->res_sn_init);
}

static inline int _is_tcp_data_packet(ch_packet_tcp_t *tcp_pkt){

    return tcp_pkt->payload_len>0;
}

static inline void
_set_tcp_state(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    if(ch_tcp_session_state_is_fin(tcp_session)||ch_tcp_session_state_is_rst(tcp_session)){
        return;
    }

    if(_is_tcp_data_packet(tcp_pkt)){
        ch_tcp_session_state_set(tcp_session,SESSION_STATE_DATA);
    }else if(is_tcp_fin_packet(tcp_pkt)){
        ch_tcp_session_state_set(tcp_session,SESSION_STATE_FIN);
    }else if(is_tcp_rst_packet(tcp_pkt)){
        ch_tcp_session_state_set(tcp_session,SESSION_STATE_RST);
    }else{
        /*do nothing*/
    }
}

static inline int 
_need_update_last_ack(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

	if(ch_tcp_session_state_is_fin(tcp_session)||ch_tcp_session_state_is_rst(tcp_session))
		return 0;
	
	if(is_tcp_fin_packet(tcp_pkt)||is_tcp_rst_packet(tcp_pkt))
		return 0;

	return 1;
}

static inline int 
_is_need_ignore(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    if(_is_tcp_data_packet(tcp_pkt))
        return 0;

    if(ch_tcp_session_state_is_fin(tcp_session)||ch_tcp_session_state_is_rst(tcp_session))
        return 0;

    return 1;
}

static inline void _update_last_ack(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

	ch_tcp_session_endpoint_t *ep = ch_tcp_session_pair_endpoint_get(tcp_session,tcp_pkt);
	if(ep){
	
		ep->last_ack = CH_MAX(ep->last_ack,
			ch_tcp_session_endpoint_offset_get(ep,tcp_pkt->recv_ack));
	}
}

static int
_tcp_session_data_process(ch_tcp_session_request_handler_t *sreq_handler,ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    _set_tcp_state(tcp_session,tcp_pkt);
	
	if(_need_update_last_ack(tcp_session,tcp_pkt))
		_update_last_ack(tcp_session,tcp_pkt);

    if(_is_need_ignore(tcp_session,tcp_pkt)){

        return -1;
    }

	return ch_tcp_session_packet_handle(sreq_handler->session_task->shandler,tcp_session,tcp_pkt);
}

static int 
_tcp_session_create(ch_tcp_session_request_handler_t *req_handler,
	ch_tcp_session_request_t *sreq,
	ch_packet_tcp_t *tcp_pkt,
	ch_tcp_app_t *app){

    ch_tcp_session_t *tcp_session;
	ch_tcp_session_pool_t *ts_pool = session_pool_get(req_handler);
			
	void *sentry = ch_tcp_app_session_entry_create(app,ts_pool->shandler->pstore);
	if(sentry == NULL)
	{	
		ch_log(CH_LOG_ERR,"create tcp app session entry failed!");
		return -1;
	}

    tcp_session = ch_tcp_session_pool_entry_create(ts_pool,sreq,app,sentry,tcp_pkt);
    
    if(tcp_session == NULL){
        ch_log(CH_LOG_ERR,"Create a tcp session failed,so drop this packet!");
        return -1;
    }

    tcp_session->session_id = ch_atomic64_add_return(cur_session_id_ptr,1)%ULONG_MAX;

    /*ok,now handle this data packet,*/
    if(_tcp_session_data_process(req_handler,tcp_session,tcp_pkt)){
        return -1;
    }

	/*ok*/
    return 0;
}

static int 
_three_way_handshake_process(ch_tcp_session_request_handler_t *req_handler,
	ch_tcp_session_request_t *sreq,
	ch_packet_tcp_t *tcp_pkt,
	ch_tcp_app_t *app){

    int rc = PROCESSOR_RET_DROP;
	int ret;

    uint32_t sent_seq = tcp_pkt->sent_seq;
    uint32_t src_ip = tcp_pkt->src_ip;
    uint32_t dst_ip = tcp_pkt->dst_ip;
    uint16_t src_port = tcp_pkt->src_port;
    uint16_t dst_port = tcp_pkt->dst_port;

	
    if(sreq == NULL){

        if(is_tcp_syn_packet(tcp_pkt)){

            /*create a new tcp session request!*/
            sreq = ch_tcp_session_request_create(req_handler->req_pool,tcp_pkt);
            if(sreq == NULL){
                ch_log(CH_LOG_ERR,"create tcp session request failed!");
                return PROCESSOR_RET_DROP;
            }

            /*init session request*/
            sreq->req_sn_init = sent_seq+1;
            if(tcp_pkt->is_ipv6){
                sreq->is_ipv6 = 1;
                memcpy(sreq->src_addr,tcp_pkt->src_addr,16);
                memcpy(sreq->dst_addr,tcp_pkt->dst_addr,16);
            }else{
                sreq->is_ipv6 = 0;
                sreq->req_ip = src_ip;
                sreq->res_ip = dst_ip;
            }

            sreq->req_port = src_port;
            sreq->res_port = dst_port;
            sreq->three_way_state = THREE_WAY_SYN;
            /*drop this packet!*/
        }
    }else{

        if(is_tcp_syn_packet(tcp_pkt)){
            /*this syn packet has been retransmited,*/
        }else if(is_tcp_syn_ack_packet(tcp_pkt)){
            if(sreq->three_way_state == THREE_WAY_SYN){
                sreq->res_sn_init = sent_seq+1;
                sreq->three_way_state = THREE_WAY_SYN_ACK;
            }else{
                /*drop it */
                //return -1;
            }
			
        }else if(_is_tcp_ack_ack_packet(sreq,tcp_pkt)){

            if(sreq->three_way_state == THREE_WAY_SYN_ACK){
                /**/
                sreq->three_way_state = THREE_WAY_ACK_ACK;
            }else{
                /*drop this packet,maybe retransmitted*/
            }

        }else if(_is_tcp_data_packet(tcp_pkt)){

            if(sreq->three_way_state == THREE_WAY_ACK_ACK){
                
                /*create a new tcp session
                 *1: create a tcp session
                 *2: free session request instance back into pool
                 * */
                ret = _tcp_session_create(req_handler,sreq,tcp_pkt,app);
                ch_tcp_session_request_free(req_handler->req_pool,sreq);

                if(ret == 0){
                    rc = PROCESSOR_RET_OK;
                }
            }else{
                /*drop this packet*/
            }

        }else{
            /*other packet(ack,fin),ignore and drop this packet!*/
        }
    }

    /*ok*/
    return rc;
}

int ch_tcp_session_request_packet_handle(ch_tcp_session_request_handler_t *req_handler,ch_packet_tcp_t *tcp_pkt){

	size_t c = 0;
    int res = PROCESSOR_RET_OK;
    ch_tcp_session_request_t *sreq = NULL;
    ch_tcp_session_t *tcp_session = NULL;
    ch_tcp_app_t *app;


	do{
		app = ch_tcp_app_find_by_port(req_handler->tcp_work->ta_pool,req_handler->session_task->shandler->pstore,tcp_pkt); 

		if(app == NULL){
			/*no application handles this packet!,skip it!*/
			res = PROCESSOR_RET_DROP;
			break;
		}

		sreq = ch_tcp_session_request_find(req_handler->req_pool,tcp_pkt);

		tcp_session = ch_tcp_session_pool_entry_find(session_pool_get(req_handler),
			tcp_pkt);

		if(tcp_session){

			/*can be assembled*/
			if(_tcp_session_data_process(req_handler,tcp_session,tcp_pkt)){
				/*process assemble failed,drop this packet!*/
				res = PROCESSOR_RET_DROP;
				break;
			}

		}else{
			/*handle three way handshake!*/
			if(_three_way_handshake_process(req_handler,sreq,tcp_pkt,app)==PROCESSOR_RET_DROP){
				/*some error ,drop this packet!*/
				res = PROCESSOR_RET_DROP;
				break;
			}
		}
	}while(0);

	c = ch_ptable_entries_timeout_free(req_handler->req_pool->tcp_session_request_tbl,
		NULL);

	if(c){
	
		ch_ptable_dump(req_handler->req_pool->tcp_session_request_tbl,stdout);

	}

    return res;

}

