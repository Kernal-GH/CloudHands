/*
 * =====================================================================================
 *
 *       Filename:  ch_packet.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月26日 11时02分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_PACKET_H
#define CH_PACKET_H

typedef struct ch_packet_info_t ch_packet_info_t;
#include <rte_mempool.h>
#include <rte_ip.h>
#include <rte_tcp.h>

#include "ch_port.h"


struct ch_packet_info_t {

    uint64_t time;
    struct rte_mbuf *mbuf;
    ch_port_t *port;

    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t sent_seq;
    uint32_t recv_ack;

    uint32_t tth_len;
    uint32_t payload_len;
};

extern void ch_packet_init(ch_packet_info_t *pinfo);

extern void ch_packet_ipinfo_init(ch_packet_info_t *pinfo,struct ipv4_hdr *iph);

extern void ch_packet_tcpinfo_init(ch_packet_info_t *pinfo,struct tcp_hdr *th);

#endif /*CH_PACKET_H*/

