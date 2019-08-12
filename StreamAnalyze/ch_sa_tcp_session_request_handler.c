/*
 *
 *      Filename: ch_sa_tcp_session_request_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-22 11:45:40
 * Last Modified: 2018-07-30 16:49:57
 */

#include "ch_sa_tcp_session_request_handler.h"
#include "ch_sa_tcp_session_handler.h"
#include "ch_log.h"
#include "ch_atomic.h"
#include "ch_tcp_session.h"
#include "ch_tcp_session_pool.h"
#include "ch_packet_record.h"
#include "ch_packet_record_session_tcp.h"

static ch_atomic64_t cur_session_id,*cur_session_id_ptr=&cur_session_id;

static inline ch_sa_session_tcp_request_entry_t*
_sa_session_tcp_request_entry_get(ch_sa_tcp_session_request_handler_t *req_handler,
	ch_tcp_session_request_t *req_session){


	void *priv_data;

	priv_data = ch_ptable_entry_priv_data(req_handler->req_pool->tcp_session_request_tbl,(void*)req_session);

	
	return (ch_sa_session_tcp_request_entry_t*)priv_data;

}

static inline uint16_t _phase_state_get(ch_tcp_session_request_t *req_session){


	if(req_session->three_way_state == THREE_WAY_SYN)
		return TCP_SESSION_PHASE_STATE_SYN;

	if(req_session->three_way_state == THREE_WAY_SYN_ACK)
		return TCP_SESSION_PHASE_STATE_SYN_ACK;


	if(req_session->three_way_state == THREE_WAY_ACK_ACK)
		return TCP_SESSION_PHASE_STATE_ACK_ACK;

	return TCP_SESSION_PHASE_STATE_SYN;
}

static void _tcp_session_request_out(ch_sa_tcp_session_request_handler_t *req_handler,
	ch_tcp_session_request_t *req_session,
	uint8_t is_timeout,
	uint16_t timeout_tv){

	size_t dlen = CH_PACKET_RECORD_SESSION_TCP_META_SIZE(0,0);
	size_t p_dlen = 0;

	ch_sa_session_task_t *sa_session_task = req_handler->session_task;
	ch_buffer_t *p_buffer = &sa_session_task->sa_buffer;

	ch_sa_session_tcp_request_entry_t *req_entry;
	ch_packet_record_t pkt_rcd;
	ch_packet_record_session_tcp_t ptmp,*p_tcp_session = &ptmp;

	if(ch_buffer_need_update(p_buffer,dlen)){
	
		if(ch_buffer_update(p_buffer,dlen)){
		
			ch_log(CH_LOG_ERR,"Cannot create buffer failed!");
			return;
		}
	}

	p_tcp_session->is_timeout = is_timeout;
	p_tcp_session->timeout_tv = timeout_tv;
	p_tcp_session->phase_state = _phase_state_get(req_session);
	p_tcp_session->session_id = 0;
	p_tcp_session->is_ipv6 = req_session->is_ipv6;
    p_tcp_session->src_ip = req_session->req_ip; 
	p_tcp_session->dst_ip = req_session->res_ip;
    p_tcp_session->src_addr = req_session->src_addr;
    p_tcp_session->dst_addr = req_session->dst_addr;
	p_tcp_session->src_port = req_session->req_port;
	p_tcp_session->dst_port = req_session->res_port;

	req_entry = _sa_session_tcp_request_entry_get(req_handler,req_session);

	p_tcp_session->req_packets = req_entry->req_packets;
	p_tcp_session->res_packets = req_entry->res_packets;
	p_tcp_session->req_bytes = 0;
	p_tcp_session->res_bytes = 0;
	p_tcp_session->req_start_time = req_entry->req_start_time;
	p_tcp_session->res_start_time = req_entry->res_start_time;
	p_tcp_session->req_last_time = req_entry->req_last_time;
	p_tcp_session->res_last_time = req_entry->res_last_time;
	p_tcp_session->req_data = NULL;
	p_tcp_session->req_dsize = 0;
	p_tcp_session->res_data = NULL;
	p_tcp_session->res_dsize = 0;


	p_dlen = ch_packet_record_session_tcp_write(p_tcp_session,p_buffer->bdata,p_buffer->b_current_size,NULL,0);

	pkt_rcd.type = PKT_RECORD_TYPE_SESSION_TCP;
	pkt_rcd.meta_data_size = dlen;
	pkt_rcd.time = p_tcp_session->req_start_time;

	ch_packet_record_put(sa_session_task->shm_fmt,
		&pkt_rcd,
		p_buffer->bdata,
		p_dlen);

}

static void _tcp_session_request_entry_timeout_cb(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data){


	ch_tcp_session_request_t *req_session = (ch_tcp_session_request_t*)entry;
	ch_sa_tcp_session_request_handler_t *req_handler = (ch_sa_tcp_session_request_handler_t*)priv_data;
	
	_tcp_session_request_out(req_handler,req_session,1,(uint16_t)tv);

}

ch_sa_tcp_session_request_handler_t *
ch_sa_tcp_session_request_handler_create(ch_sa_work_t *sa_work,ch_sa_session_task_t *session_task){

	ch_sa_tcp_session_request_handler_t *req_handler = NULL;

	req_handler = (ch_sa_tcp_session_request_handler_t*)ch_palloc(sa_work->mp,sizeof(*req_handler));

	req_handler->session_task = session_task;
	req_handler->sa_work = sa_work;

	req_handler->req_pool = ch_tcp_session_request_pool_create(sa_work->tcp_context,sizeof(ch_sa_session_tcp_request_entry_t),
		_tcp_session_request_entry_timeout_cb,
		(void*)req_handler);

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


static int
_tcp_session_data_process(ch_sa_tcp_session_request_handler_t *sreq_handler,ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt,int is_new_create){

    _set_tcp_state(tcp_session,tcp_pkt);
	
	return ch_sa_tcp_session_packet_handle(sreq_handler->session_task->tcp_session_handler,tcp_session,tcp_pkt,is_new_create);
}

static int 
_tcp_session_create(ch_sa_tcp_session_request_handler_t *req_handler,
	ch_tcp_session_request_t *sreq,
	ch_packet_tcp_t *tcp_pkt){

    ch_tcp_session_t *tcp_session;
	ch_tcp_session_pool_t *ts_pool = sa_tcp_session_pool_get(req_handler);
    tcp_session = ch_tcp_session_pool_entry_create(ts_pool,sreq,NULL,NULL,tcp_pkt);
    
    if(tcp_session == NULL){
        ch_log(CH_LOG_ERR,"Create a tcp session failed,so drop this packet!");
        return -1;
    }
	
	ch_sa_session_entry_t *sa_entry = ch_sa_session_tcp_entry_get(req_handler->session_task->tcp_session_handler,tcp_session);

	ch_sa_session_tcp_request_entry_t *req_entry = _sa_session_tcp_request_entry_get(req_handler,sreq);

	ch_sa_session_entry_init(sa_entry,req_entry);

    tcp_session->session_id = ch_atomic64_add_return(cur_session_id_ptr,1)%ULONG_MAX;

    /*ok,now handle this data packet,*/
    if(_tcp_session_data_process(req_handler,tcp_session,tcp_pkt,1)){
        return -1;
    }

	/*ok*/
    return 0;
}

static int 
_three_way_handshake_process(ch_sa_tcp_session_request_handler_t *req_handler,
	ch_tcp_session_request_t *sreq,
	ch_packet_tcp_t *tcp_pkt){

	ch_sa_session_tcp_request_entry_t *req_session_entry;

    int rc = -1;
	int ret;

    uint32_t sent_seq = tcp_pkt->sent_seq;
    uint32_t src_ip = tcp_pkt->src_ip;
    uint32_t dst_ip = tcp_pkt->dst_ip;
    uint16_t src_port = tcp_pkt->src_port;
    uint16_t dst_port = tcp_pkt->dst_port;

	uint64_t time;

    if(sreq == NULL){

        if(is_tcp_syn_packet(tcp_pkt)){

            /*create a new tcp session request!*/
            sreq = ch_tcp_session_request_create(req_handler->req_pool,tcp_pkt);
            if(sreq == NULL){
                ch_log(CH_LOG_ERR,"create tcp session request failed!");
                return -1;
            }
			
			req_session_entry = _sa_session_tcp_request_entry_get(req_handler,
				sreq);

            /*init session request*/
            sreq->req_sn_init = sent_seq+1;
            if(tcp_pkt->is_ipv6){
                memcpy(sreq->src_addr,tcp_pkt->src_addr,16);
                memcpy(sreq->dst_addr,tcp_pkt->dst_addr,16);
            }else{
                sreq->req_ip = src_ip;
                sreq->res_ip = dst_ip;
            }
            sreq->req_port = src_port;
            sreq->res_port = dst_port;
            sreq->three_way_state = THREE_WAY_SYN;
            
			/*init session entry*/
			req_session_entry->req_packets = 1;
			req_session_entry->res_packets = 0;
			time = ch_get_current_timems();

			req_session_entry->req_start_time = time;
			req_session_entry->req_last_time = time;
			req_session_entry->res_start_time = time;
			req_session_entry->res_last_time = time;
        }
    }else{

		req_session_entry = _sa_session_tcp_request_entry_get(req_handler,sreq);
		time = ch_get_current_timems();

        if(is_tcp_syn_packet(tcp_pkt)){
			req_session_entry->req_packets += 1; 
			req_session_entry->req_last_time = time;
            /*this syn packet has been retransmited,*/
        }else if(is_tcp_syn_ack_packet(tcp_pkt)){
            if(sreq->three_way_state == THREE_WAY_SYN){
                sreq->res_sn_init = sent_seq+1;
                sreq->three_way_state = THREE_WAY_SYN_ACK;
			
				req_session_entry->res_start_time = time;
				req_session_entry->res_last_time = time;
				req_session_entry->res_packets += 1;

            }else{
                /*drop it */
                //return -1;
            }
			
        }else if(_is_tcp_ack_ack_packet(sreq,tcp_pkt)){

			req_session_entry->req_packets += 1; 
			req_session_entry->req_last_time = time;

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
                ret = _tcp_session_create(req_handler,sreq,tcp_pkt);
                ch_tcp_session_request_free(req_handler->req_pool,sreq);

                if(ret == 0){
                    rc = 0;
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

int ch_sa_tcp_session_request_packet_handle(ch_sa_tcp_session_request_handler_t *req_handler,ch_packet_t *pkt){

	size_t c = 0;
    int res = 0;
    ch_tcp_session_request_t *sreq = NULL;
    ch_tcp_session_t *tcp_session = NULL;

	ch_packet_tcp_t tmp_pkt,*tcp_pkt= &tmp_pkt;

	if(ch_packet_tcp_init_from_pkt(tcp_pkt,pkt)){
	
		/*invalid packet*/
		return -1;
	}

	do{

		sreq = ch_tcp_session_request_find(req_handler->req_pool,tcp_pkt);

		tcp_session = ch_tcp_session_pool_entry_find(sa_tcp_session_pool_get(req_handler),
			tcp_pkt);

		if(tcp_session){

			/*can be assembled*/
			if(_tcp_session_data_process(req_handler,tcp_session,tcp_pkt,0)){
				/*process assemble failed,drop this packet!*/
				res = -1;
				break;
			}

		}else{
			/*handle three way handshake!*/
			if(_three_way_handshake_process(req_handler,sreq,tcp_pkt)==-1){
				/*some error ,drop this packet!*/
				res = -1;
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

