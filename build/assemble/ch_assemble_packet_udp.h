/*
 *
 *      Filename: ch_assemble_packet_udp.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-17 00:49:54
 * Last Modified: 2016-10-17 00:49:54
 */

#ifndef CH_ASSEMBLE_PACKET_UDP_H
#define CH_ASSEMBLE_PACKET_UDP_H

typedef struct ch_assemble_packet_udp_t ch_assemble_packet_udp_t;

#include "ch_assemble_packet.h"

struct ch_assemble_packet_udp_t {

	ch_assemble_packet_t as_pkt;

	uint64_t time;
	uint32_t tth_len;
	uint32_t payload_len;
	void *pdata;

	void *priv_data;
};


static inline void ch_assemble_packet_udp_init(ch_assemble_packet_udp_t *as_pkt_udp,struct rte_mbuf *m,void *priv_data){

    uint16_t mbdlen;

    struct ipv4_hdr * iph = ch_ipv4_hdr_get(m);
    
	struct udp_hdr *uh = ch_udp_hdr_get(m);

    ch_assemble_packet_udp_init2(&as_pkt_udp->as_pkt,m,iph,uh);

	
    as_pkt_udp->tth_len = m->l2_len+m->l3_len+m->l4_len;
    mbdlen = m->data_len-as_pkt_udp->tth_len;

    as_pkt_udp->payload_len = CH_MIN(mbdlen,as_pkt_udp->as_pkt.ip_dlen);
    as_pkt_udp->pdata = NULL;
    as_pkt_udp->time = ch_get_current_timems(); 
	as_pkt_udp->priv_data = priv_data;

	if(as_pkt_udp->payload_len>0){
    
        as_pkt_udp->pdata = rte_pktmbuf_mtod(m,void*)+as_pkt_udp->tth_len;
    }

}
#endif /* CH_ASSEMBLE_PACKET_UDP_H */
