/*
 *
 *      Filename: ch_tcp_session_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-05 11:06:43
 * Last Modified: 2018-07-03 17:45:00
 */

#include "ch_tcp_session_handler.h"
#include "ch_log.h"
#include "ch_string.h"
#include "ch_tcp_record.h"

static inline const char * _get_name(ch_pool_t *mp,const char *prefix,uint32_t tsk_id){

	return ch_psprintf(mp,"%s/tcp_session_%lu",prefix,(unsigned long)tsk_id);
}

static inline uint8_t _session_direct(ch_tcp_session_t *tcp_session){

    if(tcp_session->cur_ep == &tcp_session->endpoint_req){

        return SESSION_DIRECT_REQ; 
    }

    return SESSION_DIRECT_RES;
}

static inline void  
_init_record(ch_tcp_session_t *tcp_session,
	ch_tcp_record_t *tcp_rcd,
	uint8_t packet_type,
	void *data,
	uint32_t dlen){


	tcp_rcd->shm_rcd.magic = EIMAGIC;


	tcp_rcd->shm_rcd.record_size = CH_TCP_RECORD_HEADER_SIZE+dlen;

	tcp_rcd->shm_rcd.data_offset = tcp_session->cur_ep->last_offset-dlen;

	tcp_rcd->shm_rcd.data_len = dlen;
	tcp_rcd->shm_rcd.data = data; 

	tcp_rcd->packet_type = packet_type;
	tcp_rcd->session_direct = _session_direct(tcp_session);
	tcp_rcd->session_id = tcp_session->session_id;
	tcp_rcd->time = tcp_session->cur_ep->time;
	tcp_rcd->src_ip = ch_tcp_session_srcip_get(tcp_session);
	tcp_rcd->dst_ip = ch_tcp_session_dstip_get(tcp_session);
	tcp_rcd->src_port = ch_tcp_session_srcport_get(tcp_session);
	tcp_rcd->dst_port = ch_tcp_session_dstport_get(tcp_session);

	tcp_rcd->req_packets = ch_tcp_session_src_packets_get(tcp_session);
	tcp_rcd->req_bytes = ch_tcp_session_src_bytes_get(tcp_session);
	tcp_rcd->res_packets = ch_tcp_session_dst_packets_get(tcp_session);
	tcp_rcd->res_bytes = ch_tcp_session_dst_bytes_get(tcp_session);
	
	ch_tcp_session_stat_reset(tcp_session);
}


static void _tcp_session_timeout_cb(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data){

	tv = tv;

	ch_tcp_record_t tcp_rcd;
	ch_tcp_session_t *tcp_session = (ch_tcp_session_t*)entry;
	ch_tcp_session_handler_t *shandler = (ch_tcp_session_handler_t*)priv_data;

	_init_record(tcp_session,&tcp_rcd,PACKET_TYPE_CLOSE,NULL,0);

    ch_app_context_content_close(shandler->tcp_work->app_context,
		tcp_session->app,shandler->shm_fmt,(ch_shm_record_t*)(&tcp_rcd));


}

ch_tcp_session_handler_t * 
ch_tcp_session_handler_create(ch_tcp_work_t *tcp_work,ch_tcp_session_task_t *session_task){


	ch_tcp_context_t *tcp_context = tcp_work->tcp_context;

	ch_tcp_session_handler_t *shandler = NULL;

	shandler = (ch_tcp_session_handler_t*)ch_palloc(tcp_work->mp,sizeof(*shandler));

	shandler->tcp_work = tcp_work;
	shandler->session_task = session_task;
	shandler->spool = ch_tcp_session_pool_create(tcp_context,0,_tcp_session_timeout_cb,(void*)shandler);

	if(shandler->spool == NULL){
	
		ch_log(CH_LOG_ERR,"Create tcp session handler failed,cannot create tcp session pool!");
		return NULL;
	}

	shandler->shm_fmt = NULL;

	if(tcp_context->mmap_file_dir){
	
		shandler->shm_fmt = ch_shm_format_tcp_with_mmap_create(tcp_context->mp,
			_get_name(tcp_context->mp,tcp_context->mmap_file_dir,session_task->task.tsk_id),
			tcp_context->shm_size,
			tcp_context->entry_size,0,1);

	}else if(tcp_context->key){
		
		shandler->shm_fmt = ch_shm_format_tcp_with_shm_create(tcp_context->mp,
			_get_name(tcp_context->mp,tcp_context->key,session_task->task.tsk_id),
			tcp_context->proj_id,
			tcp_context->shm_size,
			tcp_context->entry_size,0,1);
	
	}
	
	if(shandler->shm_fmt == NULL){
	
		ch_log(CH_LOG_ERR,"Create tcp session handler failed,cannot create shm format!");
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

static inline void _process_fin_packet(ch_tcp_session_handler_t *shandler ch_unused,ch_tcp_session_t *tcp_session,
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




static void _tcp_session_close(ch_tcp_session_handler_t *shandler,ch_tcp_session_t *tcp_session){

	ch_tcp_record_t tcp_rcd;

	_init_record(tcp_session,&tcp_rcd,PACKET_TYPE_CLOSE,NULL,0);

    ch_app_context_content_close(shandler->tcp_work->app_context,
		tcp_session->app,shandler->shm_fmt,(ch_shm_record_t*)(&tcp_rcd));

    /*free this tcp  session */
    ch_tcp_session_pool_entry_free(shandler->spool,tcp_session);

}

static inline void _process_rst_packet(ch_tcp_session_handler_t *shandler,ch_tcp_session_t *tcp_session,
	ch_packet_tcp_t *tcp_pkt ch_unused){

    /*close session*/
    _tcp_session_close(shandler,tcp_session);
}

static void _process_data_packet(ch_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,ch_tcp_session_endpoint_t *ep,
        ch_packet_tcp_t *tcp_pkt){

	ch_tcp_record_t tcp_rcd;
    int rc;
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
            _tcp_session_close(shandler,tcp_session);
        }
    }else{
        /*offset =<last_offset<end_offset*/
        diff = ep->last_offset - offset;
        tcp_pkt->pdata+=diff;
        tcp_pkt->payload_len-=diff;
        ep->last_offset = end_offset;

		_init_record(tcp_session,&tcp_rcd,PACKET_TYPE_DATA,tcp_pkt->pdata,tcp_pkt->payload_len);

        rc = ch_app_context_content_process(shandler->tcp_work->app_context,tcp_session->app,shandler->shm_fmt,(ch_shm_record_t*)(&tcp_rcd));
		

        if(rc == PARSE_RETURN_DISCARD||rc == PARSE_RETURN_CLOSE){
            _tcp_session_close(shandler,tcp_session); 
        }else{
            df = ch_assemble_fragment_pop(&ep->as_frag,ep->last_offset);
            if(df){
				_init_record(tcp_session,&tcp_rcd,PACKET_TYPE_DATA,df->data,df->len);
        
				rc = ch_app_context_content_process(shandler->tcp_work->app_context,tcp_session->app,
					shandler->shm_fmt,(ch_shm_record_t*)(&tcp_rcd));

                if(rc == PARSE_RETURN_DISCARD||rc == PARSE_RETURN_CLOSE){
                    _tcp_session_close(shandler,tcp_session);

                }else {
                    ep->last_offset = df->offset+df->len;
                }

                ch_assemble_data_fragment_free(&ep->as_frag,df);
            }
        }
    }
}

static inline void 
_process_fin1_ack_packet(ch_tcp_session_handler_t *shandler ch_unused,
	ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt ch_unused){

    tcp_session->four_way_state = FOUR_WAY_FIN1_ACK;
}

static inline void 
_process_fin2_ack_packet(ch_tcp_session_handler_t *shandler,ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt ch_unused){

    tcp_session->four_way_state = FOUR_WAY_FIN2_ACK;

    /*close session*/
    _tcp_session_close(shandler,tcp_session);

}

int ch_tcp_session_packet_handle(ch_tcp_session_handler_t *shandler,
	ch_tcp_session_t *tcp_session,
	ch_packet_tcp_t *tcp_pkt){

	size_t c = 0;

    ch_tcp_session_endpoint_t *ep;

    do{
        /*find the tcp session's endpoint*/
        ep = ch_tcp_session_endpoint_get(tcp_session,tcp_pkt);
        if(ep == NULL){
            ch_log(CH_LOG_DEBUG,"Cannot get the tcp  session endpoint!");
            break;
        }

		ch_tcp_session_endpoint_update(ep,tcp_pkt->payload_len);

        tcp_session->cur_ep = ep;

#if 0
        /*fin packet*/
        if(is_tcp_fin_packet(tcp_pkt)){
            _process_fin_packet(shandler,tcp_session,tcp_pkt);
            break;
        }

        /*rest packet*/
        if(is_tcp_rst_packet(tcp_pkt)){
            _process_rst_packet(shandler,tcp_session,tcp_pkt);
            break;
        }
#endif
        /*data packet*/
        if(tcp_pkt->pdata){
           _process_data_packet(shandler,tcp_session,ep,tcp_pkt); 
        }

#if 0
        /*fin ack packet!*/
        if(_is_fin1_ack(tcp_session,tcp_pkt)){
            _process_fin1_ack_packet(shandler,tcp_session,tcp_pkt);
            break;
        }

        if(_is_fin2_ack(tcp_session,tcp_pkt)){
            _process_fin2_ack_packet(shandler,tcp_session,tcp_pkt);
            break;
        }
#endif

    }while(0);


	c = ch_ptable_entries_timeout_free(shandler->spool->tcp_session_tbl,
		NULL);


	if(c){
	
		ch_ptable_dump(shandler->spool->tcp_session_tbl,stdout);

	}

	/*ok*/
	return 0;
}
