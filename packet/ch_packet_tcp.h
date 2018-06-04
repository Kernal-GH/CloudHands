/*
 *
 *      Filename: ch_packet_tcp.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-13 11:27:50
 * Last Modified: 2017-12-13 11:27:50
 */

#ifndef CH_PACKET_TCP_H
#define CH_PACKET_TCP_H

#include "ch_packet.h"
#include <rte_ip.h>
#include <rte_tcp.h>

typedef struct ch_packet_tcp_t ch_packet_tcp_t;

struct ch_packet_tcp_t {

    ch_packet_t *pkt;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t ip_dlen;

    uint8_t  tcp_flags;
    uint32_t sent_seq;
    uint32_t recv_ack;

    uint32_t tth_len;
    uint32_t payload_len;

    void *pdata;
};

#define CH_TH_OFF(th)	(((th)->data_off & 0xf0) >> 4)

/* TCP flags */
#define	CH_TH_FIN      0x01
#define	CH_TH_SYN	   0x02
#define	CH_TH_RST	   0x04
#define	CH_TH_PUSH	   0x08
#define	CH_TH_ACK	   0x10
#define	CH_TH_URG	   0x20
#define CH_TH_ECNECHO  0x40	/* ECN Echo */
#define CH_TH_CWR	   0x80	/* ECN Cwnd Reduced */

static inline int is_tcp_syn_packet(ch_packet_tcp_t *tcp_pkt){
   
    return (tcp_pkt->tcp_flags&CH_TH_SYN)&&((tcp_pkt->tcp_flags&CH_TH_ACK)==0);
}

static inline int is_tcp_syn_ack_packet(ch_packet_tcp_t *tcp_pkt){
    
    return (tcp_pkt->tcp_flags&CH_TH_SYN)&&(tcp_pkt->tcp_flags&CH_TH_ACK);
}

static inline int is_tcp_fin_packet(ch_packet_tcp_t *tcp_pkt){
    
    return tcp_pkt->tcp_flags&CH_TH_FIN;
}

static inline int is_tcp_rst_packet(ch_packet_tcp_t *tcp_pkt){

    return tcp_pkt->tcp_flags&CH_TH_RST;
}

static inline int ch_packet_tcp_init_from_pkt(ch_packet_tcp_t *tcp_pkt,ch_packet_t *pkt){

    uint16_t mbdlen;
	
	const struct ipv4_hdr *iph;
	struct ipv4_hdr iph_copy;

	const struct tcp_hdr *th;
	struct tcp_hdr th_copy;
	
	iph = rte_pktmbuf_read(pkt->mbuf, pkt->l2_len, sizeof(*iph),&iph_copy);
	if(iph == NULL)
		return -1;

	th = rte_pktmbuf_read(pkt->mbuf,pkt->l2_len+pkt->l3_len, sizeof(*th), &th_copy);

	if(th == NULL)
		return -1;

    tcp_pkt->src_ip = iph->src_addr; 
    tcp_pkt->dst_ip = iph->dst_addr;
    tcp_pkt->src_port = rte_be_to_cpu_16(th->src_port);
    tcp_pkt->dst_port = rte_be_to_cpu_16(th->dst_port);
    tcp_pkt->ip_dlen = rte_cpu_to_be_16(iph->total_length)-pkt->l3_len-pkt->l4_len;

    tcp_pkt->tcp_flags = th->tcp_flags;
    tcp_pkt->sent_seq = rte_be_to_cpu_32(th->sent_seq);
    tcp_pkt->recv_ack = rte_be_to_cpu_32(th->recv_ack);

    tcp_pkt->tth_len = pkt->l2_len+pkt->l3_len+pkt->l4_len;
    mbdlen = pkt->mbuf->data_len-tcp_pkt->tth_len;

    tcp_pkt->payload_len = CH_MIN(mbdlen,tcp_pkt->ip_dlen);
    tcp_pkt->pdata = NULL;
    if(tcp_pkt->payload_len>0){
    
        tcp_pkt->pdata = rte_pktmbuf_mtod_offset(pkt->mbuf,void*,tcp_pkt->tth_len);
    }

	return 0;
}


extern void ch_packet_tcp_init(void);


#endif /*CH_PACKET_TCP_H*/
