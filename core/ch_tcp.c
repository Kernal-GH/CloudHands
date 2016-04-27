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

static int tcp_rcv(ch_protocol_type_t *protype,ch_packet_info_t *pinfo){
    uint16_t th_len = 0;
    int rc;
    struct tcp_hdr *tp_hdr;

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

    return ch_tcp_session_packet_process(tcp->tsession,pinfo);
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

    tcp->tsession = ch_tcp_session_create(context);

    if(tcp->tsession == NULL){
        ch_log(CH_LOG_ERR,"Cannot create tcp session for tcp!");
        return NULL;
    }
    return tcp;
}

void ch_tcp_destroy(ch_tcp_t *tcp){

    ch_tcp_session_destroy(tcp->tsession);
}
