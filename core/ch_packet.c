/*
 * =====================================================================================
 *
 *       Filename:  ch_packet.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月26日 15时23分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_packet.h"

void ch_packet_init(ch_packet_info_t *pinfo){

    pinfo->time = 0;
    pinfo->mbuf = NULL;
    pinfo->port = NULL;

    pinfo->src_ip = 0;
    pinfo->dst_ip = 0;
    pinfo->src_port = 0;
    pinfo->dst_port = 0;

    pinfo->sent_seq = 0;
    pinfo->recv_ack = 0;
    pinfo->tth_len = 0;
    pinfo->payload_len = 0;
}

void ch_packet_ipinfo_init(ch_packet_info_t *pinfo,struct ipv4_hdr *iph){
    
    pinfo->src_ip = iph->src_addr;
    pinfo->dst_ip = iph->dst_addr;

}

void ch_packet_tcpinfo_init(ch_packet_info_t *pinfo,struct tcp_hdr *th){

    uint32_t pdlen = 0;

    pinfo->src_port = th->src_port;
    pinfo->dst_port = th->dst_port;
    pinfo->sent_seq = rte_be_to_cpu_32(th->sent_seq);
    pinfo->recv_ack = rte_be_to_cpu_32(th->recv_ack);

    pinfo->tth_len = pinfo->mbuf->l2_len+pinfo->mbuf->l3_len+((th->data_off&0xf0)>>4)*4;
    
    pdlen = pinfo->mbuf->data_len - pinfo->tth_len;
    pdlen = pdlen == 6?0:pdlen;

    pinfo->payload_len = pdlen;
}
