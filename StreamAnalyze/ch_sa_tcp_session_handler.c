/*
 *
 *      Filename: ch_sa_tcp_session_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-28 15:02:26
 * Last Modified: 2018-08-16 09:51:28
 */

#include "ch_sa_tcp_session_handler.h"
#include "ch_log.h"
#include "ch_packet_record.h"
#include "ch_packet_record_session_tcp.h"

static inline ch_sa_session_entry_t*
_sa_session_tcp_entry_get(ch_sa_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session){


	void *priv_data;

	priv_data = ch_ptable_entry_priv_data(shandler->tcp_session_pool->tcp_session_tbl,(void*)tcp_session);

	
	return (ch_sa_session_entry_t*)priv_data;

}

static void _tcp_session_out(ch_sa_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,
	ch_sa_session_entry_t *sentry,
	uint8_t is_timeout,
	uint16_t timeout_tv,
	int is_close){

	size_t p_dlen = 0;

	ch_sa_session_task_t *sa_session_task = shandler->session_task;
	ch_buffer_t *p_buffer = &sa_session_task->sa_buffer;
	
	ch_sa_data_store_t *req_dstore = sentry->req_dstore;
	ch_sa_data_store_t *res_dstore = sentry->res_dstore;

	void *req_data = NULL,*res_data = NULL;
	size_t req_dsize = 0,res_dsize = 0;

	if(req_dstore){
		req_dsize = ch_sa_data_store_size(req_dstore);
		req_data = req_dsize?req_dstore->start:NULL;
	}

	if(res_dstore){
		res_dsize = ch_sa_data_store_size(res_dstore);
		res_data = res_dsize?res_dstore->start:NULL;
	}


	size_t meta_data_size = CH_PACKET_RECORD_SESSION_TCP_META_SIZE(req_dsize,res_dsize);

	ch_packet_record_t pkt_rcd;
	ch_packet_record_session_tcp_t ptmp,*p_tcp_session = &ptmp;


	if(ch_buffer_need_update(p_buffer,meta_data_size)){
	
		if(ch_buffer_update(p_buffer,meta_data_size)){
		
			ch_log(CH_LOG_ERR,"Cannot create buffer failed for tcp session handler!");
			ch_sa_sentry_dstore_free(sentry);
			return;
		}
	}

	p_tcp_session->is_timeout = is_timeout;
	p_tcp_session->timeout_tv = timeout_tv;
	p_tcp_session->phase_state = is_close?TCP_SESSION_PHASE_STATE_CLOSE:TCP_SESSION_PHASE_STATE_CON;
	p_tcp_session->session_id = tcp_session->session_id;
	
    p_tcp_session->is_ipv6 = ch_tcp_session_is_ipv6(tcp_session);
    p_tcp_session->src_ip = ch_tcp_session_srcip_get(tcp_session); 
	p_tcp_session->dst_ip = ch_tcp_session_dstip_get(tcp_session);
    p_tcp_session->src_addr = ch_tcp_session_srcaddr_get(tcp_session);
    p_tcp_session->dst_addr = ch_tcp_session_dstaddr_get(tcp_session);

	p_tcp_session->src_port = ch_tcp_session_srcport_get(tcp_session);
	p_tcp_session->dst_port = ch_tcp_session_dstport_get(tcp_session);

	p_tcp_session->req_packets = sentry->req_packets;
	p_tcp_session->res_packets = sentry->res_packets;
	p_tcp_session->req_bytes = sentry->req_bytes;
	p_tcp_session->res_bytes = sentry->res_bytes;
	p_tcp_session->req_start_time = sentry->req_start_time;
	p_tcp_session->res_start_time = sentry->res_start_time;
	p_tcp_session->req_last_time = sentry->req_last_time;
	p_tcp_session->res_last_time = sentry->res_last_time;

	p_tcp_session->req_data = req_data;
	p_tcp_session->req_dsize = req_dsize;
	p_tcp_session->res_data = res_data;
	p_tcp_session->res_dsize = res_dsize;

	p_dlen = ch_packet_record_session_tcp_write(p_tcp_session,p_buffer->bdata,p_buffer->b_current_size,NULL,0);

	pkt_rcd.type = PKT_RECORD_TYPE_SESSION_TCP;
	pkt_rcd.meta_data_size = meta_data_size;
	pkt_rcd.time = p_tcp_session->req_start_time;

	ch_packet_record_put(sa_session_task->shm_fmt,
		&pkt_rcd,
		p_buffer->bdata,
		p_dlen);
	
	ch_sa_sentry_dstore_free(sentry);

}

static void _tcp_session_entry_timeout_cb(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data){

	ch_tcp_session_t *tcp_session = (ch_tcp_session_t*)entry;
	ch_sa_tcp_session_handler_t *shandler = (ch_sa_tcp_session_handler_t*)priv_data;
	ch_sa_session_entry_t *sa_entry = ch_sa_session_tcp_entry_get(shandler,tcp_session);

	_tcp_session_out(shandler,tcp_session,sa_entry,1,tv,0);

}

static inline const char * _get_prefix_name(ch_pool_t *mp,uint32_t tsk_id){

	return ch_psprintf(mp,"/tcpsession/task%lu/rdb",(unsigned long)tsk_id);
}

ch_sa_tcp_session_handler_t * 
ch_sa_tcp_session_handler_create(ch_sa_work_t *sa_work,ch_sa_session_task_t *session_task){


	ch_sa_tcp_session_handler_t *shandler = NULL;

	shandler = (ch_sa_tcp_session_handler_t*)ch_palloc(sa_work->mp,sizeof(*shandler));

	shandler->sa_work = sa_work;
	shandler->session_task = session_task;

	shandler->tcp_session_pool = ch_tcp_session_pool_create(sa_work->tcp_context,
		sizeof(ch_sa_session_entry_t),
		_tcp_session_entry_timeout_cb,(void*)shandler);

	if(shandler->tcp_session_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create tcp session handler failed,cannot create tcp session pool!");
		return NULL;
	}

	return shandler;
}

static inline int _is_fin1_ack(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    return (tcp_pkt->tcp_flags&CH_TH_ACK)&&(tcp_pkt->recv_ack == tcp_session->fin1_seq+1); 
}

static inline int _is_fin2_ack(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt){

    return (tcp_pkt->tcp_flags&CH_TH_ACK)&&(tcp_pkt->recv_ack == tcp_session->fin2_seq+1); 
}

static inline void _process_fin_packet(ch_sa_tcp_session_handler_t *shandler ch_unused,ch_tcp_session_t *tcp_session,
	ch_packet_tcp_t *tcp_pkt){

    /*fin1*/
    if(tcp_session->four_way_state == FOUR_WAY_INIT){

        tcp_session->fin1_seq = tcp_pkt->sent_seq;
        tcp_session->four_way_state = FOUR_WAY_FIN1;

    }else if(tcp_session->four_way_state == FOUR_WAY_FIN1||tcp_session->four_way_state == FOUR_WAY_FIN1_ACK){
        /*fin2*/
        tcp_session->fin2_seq = tcp_pkt->sent_seq;
        tcp_session->four_way_state = FOUR_WAY_FIN2;
    }else{
        /*nothing do,maybe retrans fin packet!*/
    }

}


static void _tcp_session_close(ch_sa_tcp_session_handler_t *shandler,ch_tcp_session_t *tcp_session,ch_sa_session_entry_t *sa_entry){


	_tcp_session_out(shandler,tcp_session,sa_entry,0,0,1);
    /*free this tcp  session */
    ch_tcp_session_pool_entry_free(shandler->tcp_session_pool,tcp_session);

}

static inline void _process_rst_packet(ch_sa_tcp_session_handler_t *shandler,ch_tcp_session_t *tcp_session,
	ch_packet_tcp_t *tcp_pkt ch_unused,ch_sa_session_entry_t *sa_entry){

    /*close session*/
    _tcp_session_close(shandler,tcp_session,sa_entry);
}

static inline int _need_process_payload(ch_sa_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,
	ch_sa_session_entry_t *sa_entry,ch_packet_tcp_t *tcp_pkt){

	ch_sa_context_t *sa_context = shandler->sa_work->sa_context;

	uint32_t max_plen = sa_context->payload_data_size;
	uint32_t offset = ch_tcp_session_endpoint_offset_get(tcp_session->cur_ep,tcp_pkt->sent_seq); 

	if(offset>=max_plen)
		return 0;

	if(tcp_session->cur_ep == &tcp_session->endpoint_req){

		/*Request*/
		if(sa_entry->req_error)
			return 0;

		if(ch_sa_sentry_dstore_size(sa_entry->req_dstore)>=max_plen){
		
			return 0;
		}
	
	}else{
	
		if(sa_entry->res_error)
			return 0;

		if(ch_sa_sentry_dstore_size(sa_entry->res_dstore)>=max_plen){
		
			return 0;
		}
	}

	return 1;

}

static void _write_session_payload(ch_sa_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,ch_sa_session_entry_t *sa_entry,void *data,size_t dlen){

	size_t r_dlen = dlen;

	ch_sa_context_t *sa_context = shandler->sa_work->sa_context;
	uint32_t max_plen = sa_context->payload_data_size;
	uint32_t data_size = 0;
	ch_sa_data_store_t *dstore;
	ch_sa_data_store_pool_t *dstore_pool = shandler->session_task->dstore_pool;

	if(tcp_session->cur_ep == &tcp_session->endpoint_req){

		dstore = sa_entry->req_dstore;
		if(dstore == NULL){
		
			dstore = ch_sa_data_store_get(dstore_pool);

			if(dstore == NULL){
				sa_entry->req_error = 1;
				return;
			}

			sa_entry->req_dstore = dstore;
		}

		data_size = ch_sa_sentry_dstore_size(dstore);

		/*Request*/
		if(data_size>=max_plen){
		
			return;
		}

		if(data_size+dlen>max_plen){
		
			r_dlen = max_plen - data_size;
		}

    }else{
		
		dstore = sa_entry->res_dstore;
		if(dstore == NULL){
		
			dstore = ch_sa_data_store_get(dstore_pool);
			if(dstore == NULL){
				sa_entry->res_error = 1;
				return;
			}
			sa_entry->res_dstore = dstore;
		}

		data_size = ch_sa_sentry_dstore_size(dstore);
	
		/*Response*/
		if(data_size>=max_plen){
		
			return;
		}
		
		if(data_size+dlen>max_plen){
		
			r_dlen = max_plen - data_size;
		}

	}

	ch_sa_data_store_write(dstore,data,r_dlen);

	if(data_size>=max_plen){
	
		/*Free assemble fragments*/
		ch_tcp_session_endpoint_fin(tcp_session->cur_ep);
	}
}

static void 
_update_sa_entry(ch_sa_tcp_session_handler_t *shandler ch_unused,
	ch_tcp_session_t *tcp_session,
	ch_sa_session_entry_t *sa_entry,
	size_t dlen){

	uint64_t time = ch_get_current_timems();

	if(tcp_session->cur_ep == &tcp_session->endpoint_req){
	
		sa_entry->req_packets+=1;
		sa_entry->req_bytes += dlen;
		sa_entry->req_last_time = time;
	}else {
		sa_entry->res_packets+=1;
		sa_entry->res_bytes += dlen;
		sa_entry->res_last_time = time;
	
	}

}

static void _process_data_packet(ch_sa_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,ch_tcp_session_endpoint_t *ep,
        ch_packet_tcp_t *tcp_pkt,
		ch_sa_session_entry_t *sa_entry){

    ch_data_fragment_t *df;

    uint32_t seq,offset,end_offset,diff;
    seq = tcp_pkt->sent_seq; 
    offset = ch_tcp_session_endpoint_offset_get(ep,seq);
    end_offset = offset+tcp_pkt->payload_len;

    if(end_offset<=ep->last_offset){
        /*invalid or retrans packet!*/
    }else if(offset>ep->last_offset){
        /*unorder,assemble it!*/
        if(ch_tcp_session_endpoint_do(ep,tcp_pkt->pdata,tcp_pkt->payload_len,offset)){
            /*assemble error,maybe no memory,so close this assemble session!*/
            _tcp_session_close(shandler,tcp_session,sa_entry);
        }
    }else{
        /*offset =<last_offset<end_offset*/
        diff = ep->last_offset - offset;
        tcp_pkt->pdata+=diff;
        tcp_pkt->payload_len-=diff;
        ep->last_offset = end_offset;

		_write_session_payload(shandler,tcp_session,sa_entry,tcp_pkt->pdata,tcp_pkt->payload_len);

		df = ch_assemble_fragment_pop(&ep->as_frag,ep->last_offset);
		if(df){
			
			_write_session_payload(shandler,tcp_session,sa_entry,df->data,df->len);
	
			ep->last_offset = df->offset+df->len;

			ch_assemble_data_fragment_free(&ep->as_frag,df);
		}
    }
}

static inline void 
_process_fin1_ack_packet(ch_sa_tcp_session_handler_t *shandler ch_unused,
	ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt ch_unused){

    tcp_session->four_way_state = FOUR_WAY_FIN1_ACK;
}

static inline void 
_process_fin2_ack_packet(ch_sa_tcp_session_handler_t *shandler,ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt ch_unused,ch_sa_session_entry_t *sa_entry){

    tcp_session->four_way_state = FOUR_WAY_FIN2_ACK;

    /*close session*/
    _tcp_session_close(shandler,tcp_session,sa_entry);

}

int ch_sa_tcp_session_packet_handle(ch_sa_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,
	ch_packet_tcp_t *tcp_pkt,
	int is_new_create ch_unused){

	//size_t c = 0;
	ch_sa_session_entry_t *sa_entry;

    ch_tcp_session_endpoint_t *ep;

    do{
        /*find the tcp session's endpoint*/
        ep = ch_tcp_session_endpoint_get(tcp_session,tcp_pkt);
        if(ep == NULL){
            ch_log(CH_LOG_DEBUG,"Cannot get the tcp  session endpoint!");
            break;
        }

        tcp_session->cur_ep = ep;

		sa_entry = ch_sa_session_tcp_entry_get(shandler,tcp_session);
		_update_sa_entry(shandler,tcp_session,sa_entry,tcp_pkt->payload_len);

        /*fin packet*/
        if(is_tcp_fin_packet(tcp_pkt)){
            _process_fin_packet(shandler,tcp_session,tcp_pkt);
            break;
        }

        /*rest packet*/
        if(is_tcp_rst_packet(tcp_pkt)){
            _process_rst_packet(shandler,tcp_session,tcp_pkt,sa_entry);
            break;
        }
        /*data packet*/
        if(tcp_pkt->pdata&&_need_process_payload(shandler,tcp_session,sa_entry,tcp_pkt)){
           _process_data_packet(shandler,tcp_session,ep,tcp_pkt,sa_entry); 
        }

        /*fin ack packet!*/
        if(_is_fin1_ack(tcp_session,tcp_pkt)){
            _process_fin1_ack_packet(shandler,tcp_session,tcp_pkt);
            break;
        }

        if(_is_fin2_ack(tcp_session,tcp_pkt)){
            _process_fin2_ack_packet(shandler,tcp_session,tcp_pkt,sa_entry);
            break;
        }

    }while(0);


	//c =
	ch_ptable_entries_timeout_free(shandler->tcp_session_pool->tcp_session_tbl,NULL);

#if 0
	if(c){
	
		ch_ptable_dump(shandler->tcp_session_pool->tcp_session_tbl,stdout);

	}

#endif

	/*ok*/
	return 0;
}
