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
#include "ch_util.h"
#include "ch_assemble_packet.h"
#include "ch_assemble_packet_tcp.h"
#include "ch_assemble_pool_tcp.h"
#include "ch_packets_interface.h"

static void _tcp_dump(ch_assemble_packet_tcp_t *tcp_pkt){

    const char *flgs = ".";
    char srcip[32] = {0};
    char dstip[32] = {0};
    ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)tcp_pkt;

    ch_ipaddr_str_get(srcip,32,as_pkt->src_ip);
    ch_ipaddr_str_get(dstip,32,as_pkt->dst_ip);

    if(is_tcp_syn_packet(tcp_pkt)||is_tcp_syn_ack_packet(tcp_pkt)){
        flgs = "S";
    }else if(is_tcp_fin_packet(tcp_pkt)){
        flgs = "F";
    }else if(is_tcp_rst_packet(tcp_pkt)){
        flgs = "C";
    }

    printf("%s:%lu--->%s:%lu,seq:%lu,ack:%lu,dlen:%lu\n,pdlen:%lu,flags:[%s]\n",srcip,
            (unsigned long)as_pkt->src_port,
            dstip,
            (unsigned long)as_pkt->dst_port,
            (unsigned long)tcp_pkt->sent_seq,
            (unsigned long)tcp_pkt->recv_ack,
            (unsigned long)tcp_pkt->payload_len,
            (unsigned long)as_pkt->ip_dlen,
            flgs);
}

static int tcp_rcv(ch_protocol_type_t *protype,struct rte_mbuf *mbuf){
    uint16_t th_len = 0;
    int rc;
    struct tcp_hdr *tp_hdr;
    ch_assemble_packet_tcp_t tcp_pkt;

    ch_tcp_t *tcp = (ch_tcp_t *)protype->proto_type_priv;

    tp_hdr = ch_tcp_hdr_get(mbuf);

    th_len = CH_TH_OFF(tp_hdr);
    
    /*Invalid tcp packet!!*/
    if(th_len<sizeof(struct tcp_hdr)/4){ 
        ch_log(CH_LOG_ERR,"Invalid tcp packet:%u\n",th_len);
        return PROCESSOR_RET_DROP;
    }

    /*update l4_len mbuf*/
    mbuf->l4_len = th_len*4;
    ch_assemble_packet_tcp_init(&tcp_pkt,mbuf);

	ch_packets_interface_put(tcp->context->pin,(ch_assemble_packet_t*)&tcp_pkt,PROTO_TCP);
   // _tcp_dump(&tcp_pkt);
    
    return ch_assemble_pool_packet_process(tcp->asp,(ch_assemble_packet_t *)&tcp_pkt);

}

ch_protocol_type_t tcp_proto = {

    CH_IPPROTO_TCP,
    NULL,
    tcp_rcv
};

ch_tcp_t * ch_tcp_create(ch_context_t *context,ch_ipv4_t *ipv4){

    ch_tcp_t *tcp = (ch_tcp_t*)apr_palloc(context->mp,sizeof(ch_tcp_t));

    if(!tcp){

        ch_log(CH_LOG_ERR,"no memory to create tcp instance!");
        return NULL;
    }

    tcp->context = context;

    tcp_proto.proto_type_priv = (void*)tcp;

    /*register tcp protocol into ipv4,*/
    ch_ipv4_proto_register(ipv4,&tcp_proto);

    tcp->asp = ch_assemble_pool_tcp_create(context);

    if(tcp->asp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create tcp assemble pool for tcp!");
        return NULL;
    }

    return tcp;
}

void ch_tcp_destroy(ch_tcp_t *tcp){

    ch_assemble_pool_tcp_destroy(tcp->asp);

}
