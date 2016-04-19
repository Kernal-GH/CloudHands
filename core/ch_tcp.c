/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月29日 14时49分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_constants.h"
#include "ch_tcp.h"
#include "ch_log.h"
#include "ch_protocol_type.h"
#include "ch_packet.h"
#include "ch_util.h"

static int _assemble_session_find(ch_tcp_t *tcp,ch_assemble_session_t **ass,ch_assemble_task_t **astask,
        ch_packet_info_t *pinfo){

    ch_assemble_task_t **astsks,*astsk;
    ch_assemble_session_t *as;
    int i;
    *ass = NULL;
    *astask = NULL;

    astsks = (ch_assemble_task_t**)(tcp->req_pool->assemble_tasks->elts);
    
    for(i = 0; i<tcp->req_pool->assemble_tasks->nelts;i++){

        astsk = astsks[i];
        as = ch_assemble_task_session_find(astsk,pinfo);
        if(as){
            /*Found*/
            *ass = as;
            *astask = astsk;
            return 0;
        }
    }
    /*no found!*/
    return -1;
} 

static inline int _is_tcp_ack_ack_packet(ch_session_request_t *sreq,struct tcp_hdr *th){

    uint32_t recv_ack = rte_be_to_cpu_32(th->recv_ack);
    return (th->tcp_flags&CH_TH_ACK)&&(sreq->three_way_state == THREE_WAY_SYN_ACK)&&(recv_ack == sreq->res_sn_init);
}

static inline int _is_tcp_data_packet(struct tcp_hdr *th,ch_packet_info_t *pinfo){

    return pinfo->payload_len>0;
}

static inline int _is_need_ignore(ch_assemble_session_t *ass,ch_packet_info_t *pinfo,struct tcp_hdr *th){

    if(_is_tcp_data_packet(th,pinfo))
        return 0;

    if(session_state_is_fin(ass)||session_state_is_rst(ass))
        return 0;

    return 1;
}

static inline void _set_assemble_state(ch_assemble_session_t *ass,ch_packet_info_t *pinfo,struct tcp_hdr *th){

    if(session_state_is_fin(ass)||session_state_is_rst(ass)){
        return;
    }

    if(_is_tcp_data_packet(th,pinfo)){
        ch_assemble_session_state_set(ass,SESSION_STATE_DATA);
    }else if(session_state_is_fin(ass)){
        ch_assemble_session_state_set(ass,SESSION_STATE_FIN);
    }else if(session_state_is_rst(ass)){
        ch_assemble_session_state_set(ass,SESSION_STATE_RST);
    }else{
        /*do nothing*/
    }
}

static int _tcp_data_assemble_process(ch_assemble_task_t *astask,ch_assemble_session_t *ass,ch_packet_info_t *pinfo,struct tcp_hdr *th){

    if(_is_need_ignore(ass,pinfo,th)){

        return -1;
    }

    /*put this packet into assemble tasks and waiting to process it!*/
    if(ch_assemble_task_pkt_put(astask,pinfo)){
        /*failed ,so need to update assemble session's need_end == 1,*/
        ass->need_end = 1;
        return -1;
    }

    _set_assemble_state(ass,pinfo,th);

    return 0;
}

static int _assemble_session_create(ch_tcp_t *tcp,ch_session_request_t *sreq,ch_packet_info_t *pinfo,struct tcp_hdr *th){

    ch_assemble_task_t *astask;
    ch_assemble_session_t *ass;

    astask = ch_assemble_task_get_from_request_pool(tcp->req_pool);
    if(astask == NULL){
        ch_log(CH_LOG_ERR,"Cannot get an assemble task to handle this packets,so drop it!");
        return -1;
    }

    ass = ch_assemble_task_session_create(astask,sreq,pinfo);
    if(ass == NULL){
        ch_log(CH_LOG_ERR,"Create an assemble session failed,so drop this packet!");
        return -1;
    }

    /*ok,now handle this data packet,*/
    if(_tcp_data_assemble_process(astask,ass,pinfo,th)){
        return -1;
    }

    return 0;
}

static int _three_way_handshake_process(ch_tcp_t *tcp,ch_session_request_t *sreq,ch_packet_info_t *pinfo,struct tcp_hdr *th){

    int rc;

    uint32_t sent_seq = pinfo->sent_seq;
    uint32_t src_ip = pinfo->src_ip;
    uint32_t dst_ip = pinfo->dst_ip;
    uint16_t src_port = pinfo->src_port;
    uint16_t dst_port = pinfo->dst_port;

    if(sreq == NULL){

        if(is_tcp_syn_packet(th)){

            /*create a new session request!*/
            sreq = ch_session_request_create(tcp->req_pool,pinfo);
            if(sreq == NULL){
                ch_log(CH_LOG_ERR,"create session request failed!");
                return -1;
            }

            /*init session request*/
            sreq->req_sn_init = sent_seq+1;
            sreq->req_ip = src_ip;
            sreq->req_port = src_port;
            sreq->res_ip = dst_ip;
            sreq->res_port = dst_port;
            sreq->three_way_state = THREE_WAY_SYN;
        }else{
            /*drop this packet!*/
            return -1;
        }
    }else{

        if(is_tcp_syn_packet(th)){
            /*this syn packet has been retransmited,*/
            return -1;
        }else if(is_tcp_syn_ack_packet(th)){
            if(sreq->three_way_state == THREE_WAY_SYN){
                sreq->res_sn_init = sent_seq+1;
                sreq->three_way_state = THREE_WAY_SYN_ACK;
            }else{
                /*drop it */
                return -1;
            }
        }else if(_is_tcp_ack_ack_packet(sreq,th)){

            if(sreq->three_way_state == THREE_WAY_SYN_ACK){
                /**/
                sreq->three_way_state = THREE_WAY_ACK_ACK;
            }else{
                /*drop this packet,maybe retransmitted*/
                return -1;
            }

        }else if(_is_tcp_data_packet(th,pinfo)){

            if(sreq->three_way_state == THREE_WAY_ACK_ACK){
                
                /*create a new assemble session
                 *1: get an assemble task
                 *2: create an assemble session
                 *3: free session request instance back into pool
                 * */
                rc = _assemble_session_create(tcp,sreq,pinfo,th);
                ch_session_request_free(tcp->req_pool,sreq);

                if(rc){
                    return -1;
                }
            }else{
                /*drop this packet*/
                return -1;
            }

        }else{
            /*other packet(ack,fin),ignore and drop this packet!*/
            return -1;
        }
    }

    /*ok*/
    return 0;
}



static int tcp_rcv(ch_protocol_type_t *protype,ch_packet_info_t *pinfo){
    uint16_t th_len = 0;
    int rc;

    struct tcp_hdr *tp_hdr;

    ch_session_request_t *sreq = NULL;
    ch_assemble_session_t *asession = NULL;
    ch_assemble_task_t *astask;

    ch_tcp_t *tcp = (ch_tcp_t *)protype->proto_type_priv;

    tp_hdr = ch_tcp_hdr_get(pinfo->mbuf);

    ch_packet_tcpinfo_init(pinfo,tp_hdr);

    th_len = CH_TH_OFF(tp_hdr);
    
    /*Invalid tcp packet!!*/
    if(th_len<sizeof(struct tcp_hdr)/4){ 

        return PROCESSOR_RET_DROP;
    }

    /*update l4_len mbuf*/
    pinfo->mbuf->l4_len = th_len*4;

    if(ch_app_context_recognize_by_port(tcp->context->app_context,
                rte_be_to_cpu_16(pinfo->src_port),
                rte_be_to_cpu_16(pinfo->dst_port))==0){
        /*no application handles this packet!,skip it!*/
        return PROCESSOR_RET_DROP;
    }

    sreq = ch_session_request_find(tcp->req_pool,pinfo);
    rc = _assemble_session_find(tcp,&asession,&astask,pinfo);

    if(rc == 0){
        /*can be assembled*/
        if(_tcp_data_assemble_process(astask,asession,pinfo,tp_hdr)){
            /*process assemble failed,drop this packet!*/
            return PROCESSOR_RET_DROP;
        }
    }else{
        /*handle three way handshake!*/
        if(_three_way_handshake_process(tcp,sreq,pinfo,tp_hdr)){
            /*some error ,drop this packet!*/
            return PROCESSOR_RET_DROP;
        }
    }
    return PROCESSOR_RET_OK;
}

ch_protocol_type_t tcp_proto = {

    CH_IPPROTO_TCP,
    NULL,
    tcp_rcv
};

ch_tcp_t * ch_tcp_create(ch_context_t *context,ch_ipv4_t *ipv4){

    ch_tcp_t *tcp = (ch_tcp_t*)apr_palloc(context->mp,sizeof(ch_ipv4_t));

    if(!tcp){

        ch_log(CH_LOG_ERR,"no memory to create tcp instance!");
        return NULL;
    }

    tcp->context = context;

    tcp_proto.proto_type_priv = (void*)tcp;

    /*register tcp protocol into ipv4,*/
    ch_ipv4_proto_register(ipv4,&tcp_proto);

    tcp->req_pool = ch_session_request_pool_create(context);
    if(tcp->req_pool == NULL){
        
        ch_log(CH_LOG_ERR,"Create session request pool instance failed!");
        return NULL;
    }

    return tcp;
}

void ch_tcp_destroy(ch_tcp_t *tcp){
    
    if(tcp->req_pool){
        ch_session_request_pool_destroy(tcp->req_pool);
        tcp->req_pool = NULL;
    }

}
