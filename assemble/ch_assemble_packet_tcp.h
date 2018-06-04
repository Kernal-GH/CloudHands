
/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_packet_tcp.h
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

#ifndef CH_ASSEMBLE_PACKET_TCP_H
#define CH_ASSEMBLE_PACKET_TCP_H

typedef struct ch_assemble_packet_tcp_t ch_assemble_packet_tcp_t;

#include "ch_log.h"
#include "ch_assemble_packet.h"

struct ch_assemble_packet_tcp_t {

    ch_assemble_packet_t as_pkt;

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

static inline int is_tcp_syn_packet(ch_assemble_packet_tcp_t *tas_pkt){
   
    return (tas_pkt->tcp_flags&CH_TH_SYN)&&((tas_pkt->tcp_flags&CH_TH_ACK)==0);
}

static inline int is_tcp_syn_ack_packet(ch_assemble_packet_tcp_t *tas_pkt){
    
    return (tas_pkt->tcp_flags&CH_TH_SYN)&&(tas_pkt->tcp_flags&CH_TH_ACK);
}

static inline int is_tcp_fin_packet(ch_assemble_packet_tcp_t *tas_pkt){
    
    return tas_pkt->tcp_flags&CH_TH_FIN;
}

static inline int is_tcp_rst_packet(ch_assemble_packet_tcp_t *tas_pkt){

    return tas_pkt->tcp_flags&CH_TH_RST;
}

static inline void ch_assemble_packet_tcp_init(ch_assemble_packet_tcp_t *as_pkt_tcp,struct rte_mbuf *m){

    uint16_t mbdlen;

    struct ipv4_hdr * iph = ch_ipv4_hdr_get(m);
    struct tcp_hdr *th = ch_tcp_hdr_get(m);
    ch_assemble_packet_init2(&as_pkt_tcp->as_pkt,m,iph,th);

    as_pkt_tcp->tcp_flags = th->tcp_flags;
    as_pkt_tcp->sent_seq = rte_be_to_cpu_32(th->sent_seq);
    as_pkt_tcp->recv_ack = rte_be_to_cpu_32(th->recv_ack);

    as_pkt_tcp->tth_len = m->l2_len+m->l3_len+m->l4_len;
    mbdlen = m->data_len-as_pkt_tcp->tth_len;

    as_pkt_tcp->payload_len = CH_MIN(mbdlen,as_pkt_tcp->as_pkt.ip_dlen);
    as_pkt_tcp->pdata = NULL;
    if(as_pkt_tcp->payload_len>0){
    
        as_pkt_tcp->pdata = rte_pktmbuf_mtod(m,void*)+as_pkt_tcp->tth_len;
    }
}
#endif /*CH_ASSEMBLE_PACKET_TCP_H*/
