/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_packet.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月30日 14时03分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_ASSEMBLE_PACKET_H
#define CH_ASSEMBLE_PACKET_H

typedef struct ch_assemble_packet_t ch_assemble_packet_t;
#include <stdint.h>
#include "ch_util.h"
#include "ch_dpdk_util.h"

struct ch_assemble_packet_t {

    struct rte_mbuf *mbuf;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t ip_dlen;
};

static inline void ch_assemble_packet_init(ch_assemble_packet_t *as_pkt,struct rte_mbuf *mbuf){

    struct ipv4_hdr * iph = ch_ipv4_hdr_get(mbuf);
    struct tcp_hdr *th = ch_tcp_hdr_get(mbuf);

    as_pkt->mbuf = mbuf;
    as_pkt->src_ip = iph->src_addr; 
    as_pkt->dst_ip = iph->dst_addr;
    as_pkt->src_port = rte_be_to_cpu_16(th->src_port);
    as_pkt->dst_port = rte_be_to_cpu_16(th->dst_port);
    as_pkt->ip_dlen = rte_cpu_to_be_16(iph->total_length)-mbuf->l3_len-mbuf->l4_len;
}

static inline void ch_assemble_packet_init2(ch_assemble_packet_t *as_pkt,struct rte_mbuf *m,struct ipv4_hdr *iph,struct tcp_hdr *th){

    as_pkt->mbuf = m;
    as_pkt->src_ip = iph->src_addr; 
    as_pkt->dst_ip = iph->dst_addr;
    as_pkt->src_port = rte_be_to_cpu_16(th->src_port);
    as_pkt->dst_port = rte_be_to_cpu_16(th->dst_port);
    as_pkt->ip_dlen = rte_cpu_to_be_16(iph->total_length)-m->l3_len-m->l4_len;
}

static inline void ch_assemble_packet_udp_init1(ch_assemble_packet_t *as_pkt,struct rte_mbuf *mbuf){

    struct ipv4_hdr * iph = ch_ipv4_hdr_get(mbuf);
	struct udp_hdr *uh = ch_udp_hdr_get(mbuf);

    as_pkt->mbuf = mbuf;
    as_pkt->src_ip = iph->src_addr; 
    as_pkt->dst_ip = iph->dst_addr;
    as_pkt->src_port = rte_be_to_cpu_16(uh->src_port);
    as_pkt->dst_port = rte_be_to_cpu_16(uh->dst_port);
    as_pkt->ip_dlen = rte_cpu_to_be_16(iph->total_length)-mbuf->l3_len-mbuf->l4_len;
}

static inline void ch_assemble_packet_udp_init2(ch_assemble_packet_t *as_pkt,struct rte_mbuf *m,struct ipv4_hdr *iph,struct udp_hdr *uh){

    as_pkt->mbuf = m;
    as_pkt->src_ip = iph->src_addr; 
    as_pkt->dst_ip = iph->dst_addr;
    as_pkt->src_port = rte_be_to_cpu_16(uh->src_port);
    as_pkt->dst_port = rte_be_to_cpu_16(uh->dst_port);
    as_pkt->ip_dlen = rte_cpu_to_be_16(iph->total_length)-m->l3_len-m->l4_len;
}

#endif /*CH_ASSEMBLE_PACKET_H*/
