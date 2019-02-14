/*
 *
 *      Filename: ch_packet_ipv4.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-13 10:18:53
 * Last Modified: 2017-12-13 10:18:53
 */

#ifndef CH_PACKET_IPV4_H
#define CH_PACKET_IPV4_H

#include "ch_packet.h"
#include <rte_ip.h>

extern void ch_packet_ipv4_init(void);

static inline uint32_t ch_packet_ipv4_srcip(ch_packet_t *pkt){

	const struct ipv4_hdr *ip4h;
	struct ipv4_hdr ip4h_copy;

	ip4h = rte_pktmbuf_read(pkt->mbuf, pkt->l2_len, sizeof(*ip4h),&ip4h_copy);

	if(unlikely(ip4h == NULL))
		return 0;

	return ip4h->src_addr;
}

static inline uint32_t ch_packet_ipv4_dstip(ch_packet_t *pkt){

	const struct ipv4_hdr *ip4h;
	struct ipv4_hdr ip4h_copy;

	ip4h = rte_pktmbuf_read(pkt->mbuf, pkt->l2_len, sizeof(*ip4h),&ip4h_copy);

	if(unlikely(ip4h == NULL))
		return 0;

	return ip4h->dst_addr;
}

#endif /*CH_PACKET_IPV4_H*/
