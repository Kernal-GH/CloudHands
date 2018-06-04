/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_tcp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月30日 15时52分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_log.h"
#include "ch_assemble_tcp.h"
#include "ch_assemble_packet_tcp.h"

static inline int _is_fin1_ack(ch_assemble_session_tcp_t *ass,ch_assemble_packet_tcp_t *tcp_pkt){

    return (tcp_pkt->tcp_flags&CH_TH_ACK)&&(tcp_pkt->recv_ack == ass->fin1_seq+1); 
}

static inline int _is_fin2_ack(ch_assemble_session_tcp_t *ass,ch_assemble_packet_tcp_t *tcp_pkt){

    return (tcp_pkt->tcp_flags&CH_TH_ACK)&&(tcp_pkt->recv_ack == ass->fin2_seq+1); 
}

static void _assemble_session_close(ch_assemble_tcp_t  *tas,ch_assemble_session_tcp_t *ass){


    ch_app_context_content_close(tas->aspt->app_tcp_context,ass->app,(void*)ass);

    /*free this tcp assemble session */
    ch_assemble_session_pool_tcp_entry_free(tas->asspt,ass);
}

static inline void _process_fin_packet(ch_assemble_tcp_t *tas,ch_assemble_session_tcp_t *ass,ch_assemble_packet_tcp_t *tcp_pkt){

    /*fin1*/
    if(ass->four_way_state == FOUR_WAY_INIT){

        ass->fin1_seq = tcp_pkt->sent_seq;
        ass->four_way_state = FOUR_WAY_FIN1;

    }else if(ass->four_way_state == FOUR_WAY_FIN1||ass->four_way_state == FOUR_WAY_FIN1_ACK){
        /*fin2*/
        ass->fin2_seq = tcp_pkt->sent_seq;
        ass->four_way_state = FOUR_WAY_FIN2;
    }else{
        /*nothing do,maybe retrans fin packet!*/
    }

}

static void _process_rst_packet(ch_assemble_tcp_t *tas,ch_assemble_session_tcp_t *ass,ch_assemble_packet_tcp_t *tcp_pkt){

    /*close session*/
    _assemble_session_close(tas,ass);
}

static void _process_data_packet(ch_assemble_tcp_t *tas,ch_assemble_session_tcp_t *ass,ch_assemble_session_endpoint_t *ep,
        ch_assemble_packet_tcp_t *tcp_pkt){

    int rc;
    ch_data_fragment_t *df;

    uint32_t seq,offset,end_offset,diff;
    seq = tcp_pkt->sent_seq; 
    offset = ch_assemble_session_endpoint_offset_get(ep,seq);
    end_offset = offset+tcp_pkt->payload_len;

    if(offset<0||end_offset<=ep->last_offset){
        /*invalid or retrans packet!*/
    }else if(offset>ep->last_offset){
        /*unorder,assemble it!*/
        if(ch_assemble_session_endpoint_do(ep,tcp_pkt->pdata,tcp_pkt->payload_len,offset)){
            /*assemble error,maybe no memory,so close this assemble session!*/
            _assemble_session_close(tas,ass);
        }
    }else{
        /*offset =<last_offset<end_offset*/
        diff = ep->last_offset - offset;
        tcp_pkt->pdata+=diff;
        tcp_pkt->payload_len-=diff;
        ep->last_offset = end_offset;

        rc = ch_app_context_content_process(tas->aspt->app_tcp_context,ass->app,tcp_pkt->pdata,tcp_pkt->payload_len,(void*)ass);
        if(rc == PARSE_RETURN_DISCARD||rc == PARSE_RETURN_CLOSE){
            _assemble_session_close(tas,ass); 
        }else{
            df = ch_assemble_fragment_pop(&ep->as_frag,ep->last_offset);
            if(df){
                rc = ch_app_context_content_process(tas->aspt->app_tcp_context,ass->app,df->data,df->len,(void*)ass);
                if(rc == PARSE_RETURN_DISCARD||rc == PARSE_RETURN_CLOSE){
                    _assemble_session_close(tas,ass);
                }else {
                    ep->last_offset = df->offset+df->len;
                }

                ch_assemble_data_fragment_free(df);
            }
        }
    }
}

static inline void _process_fin1_ack_packet(ch_assemble_tcp_t *tas,ch_assemble_session_tcp_t *ass,ch_assemble_packet_tcp_t *tcp_pkt){

    ass->four_way_state = FOUR_WAY_FIN1_ACK;
}

static void _process_fin2_ack_packet(ch_assemble_tcp_t *tas,ch_assemble_session_tcp_t *ass,ch_assemble_packet_tcp_t *tcp_pkt){

    ass->four_way_state = FOUR_WAY_FIN2_ACK;

    /*close session*/
    _assemble_session_close(tas,ass);

}

static int _asseble_session_need_free(ch_ptable_entry_t *entry,void *priv_data){

	/* unused */
	priv_data = priv_data;

	ch_assemble_session_tcp_t *ass = (ch_assemble_session_tcp_t*)entry;
    ch_assemble_session_endpoint_t *req = &ass->endpoint_req; 
    ch_assemble_session_endpoint_t *res = &ass->endpoint_res;

	/*
	if(req->last_ack>req->last_offset||res->last_ack>res->last_offset)
		return 1;
		*/

	return 0;
}

static void do_assemble_tcp(ch_assemble_t *as,struct rte_mbuf *mbuf){

	size_t c = 0;
	char b[128];

    ch_assemble_packet_tcp_t tcp_pkt;
    ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)&tcp_pkt;

    ch_assemble_session_tcp_t *ass;
    ch_assemble_session_endpoint_t *ep;
    ch_assemble_tcp_t *tas = (ch_assemble_tcp_t*)as;

    ch_assemble_packet_tcp_init(&tcp_pkt,mbuf);

    do{
        /*find the assemble session*/
        ass = ch_assemble_session_pool_tcp_entry_find(tas->asspt,as_pkt);
        if(ass == NULL){
            ch_log(CH_LOG_DEBUG,"The tcp assemble session no existed !");
            break;
        }
        /*find the assemble session's endpoint*/
        ep = ch_assemble_session_tcp_endpoint_get(ass,as_pkt);
        if(ep == NULL){
            ch_log(CH_LOG_DEBUG,"Cannot get the tcp assemble session endpoint!");
            break;
        }
        ass->cur_ep = ep;

        /*fin packet*/
        if(is_tcp_fin_packet(&tcp_pkt)){
            _process_fin_packet(tas,ass,&tcp_pkt);
            break;
        }

        /*rest packet*/
        if(is_tcp_rst_packet(&tcp_pkt)){
            _process_rst_packet(tas,ass,&tcp_pkt);
            break;
        }

        /*data packet*/
        if(tcp_pkt.pdata){
           _process_data_packet(tas,ass,ep,&tcp_pkt); 
        }

        /*fin ack packet!*/
        if(_is_fin1_ack(ass,&tcp_pkt)){
            _process_fin1_ack_packet(tas,ass,&tcp_pkt);
            break;
        }

        if(_is_fin2_ack(ass,&tcp_pkt)){
            _process_fin2_ack_packet(tas,ass,&tcp_pkt);
            break;
        }

    }while(0);

    /*free mbuf!*/
    rte_pktmbuf_free(mbuf);

	ch_assemble_session_pool_lock(tas->asspt);
	c = ch_ptable_entries_timeout_free(tas->asspt->as_tbl,
		_asseble_session_need_free);

	ch_assemble_session_pool_unlock(tas->asspt);

	if(c){
	
		fprintf(stdout,"Free assemble tcp session:%lu\n",(unsigned long)c);
		ch_ptable_dump(tas->asspt->as_tbl,stdout);

	}

}

ch_assemble_t * ch_assemble_tcp_create(ch_assemble_pool_tcp_t *aspt,int assemble_id){

    ch_assemble_context_tcp_t *tcp_context = &aspt->tcp_context;

    ch_assemble_tcp_t * tas = NULL;

    tas = (ch_assemble_tcp_t*)apr_palloc(tcp_context->context->mp,sizeof(*tas));

    tas->aspt = aspt;

    if(-1 == ch_assemble_init(tcp_context->context,(ch_assemble_t*)tas,assemble_id,tcp_context->mmap_file_dir,"tcp",
                tcp_context->mmap_file_size,
                tcp_context->mmap_file_entry_size,
                do_assemble_tcp))
    {
    
        ch_log(CH_LOG_ERR,"Create tcp assemble failed!");
        return NULL;
    }

    tas->asspt = ch_assemble_session_pool_tcp_create((ch_assemble_t*)tas,tcp_context); 

    if(tas->asspt == NULL){
    
        ch_assemble_fin((ch_assemble_t*)tas);
        ch_log(CH_LOG_ERR,"Create tcp assemble sessions pool failed!");
        return NULL;
    }

    return (ch_assemble_t*)tas;
}

void ch_assemble_tcp_destroy(ch_assemble_tcp_t *tas){

    ch_assemble_session_pool_tcp_destroy(tas->asspt);
    ch_assemble_fin((ch_assemble_t*)tas);
}
