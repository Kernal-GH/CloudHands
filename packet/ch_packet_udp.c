/*
 *
 *      Filename: ch_packet_udp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-03 10:07:14
 * Last Modified: 2018-04-21 17:27:17
 */

#include "ch_packet_udp.h"
#include <rte_udp.h>
#include "ch_log.h"
#include "ch_jhash.h"

static int _packet_udp_parse(ch_packet_t *pkt){

	const struct ipv4_hdr *iph;
	struct ipv4_hdr iph_copy;
	const struct udp_hdr *uh;
	struct udp_hdr uh_copy;
	
	struct rte_mbuf *mbuf = pkt->mbuf;
	uint16_t udp_hlen = sizeof(*uh);

	pkt->pkt_type = PKT_TYPE_UDP;

	iph = rte_pktmbuf_read(mbuf, pkt->l2_len, sizeof(*iph),&iph_copy);
	uh = rte_pktmbuf_read(mbuf,pkt->parse_off, sizeof(*uh), &uh_copy);

	if (unlikely(uh == NULL||iph == NULL)){
	
		ch_log(CH_LOG_ERR,"Invalid udp packet!");
		return PKT_PARSE_DROP;
	}


	pkt->l4_len = udp_hlen;
	pkt->parse_off += pkt->l4_len;

	pkt->hash = ch_jhash_4words_sort(iph->src_addr,
		iph->dst_addr,
		rte_be_to_cpu_16(uh->src_port),
		rte_be_to_cpu_16(uh->dst_port),
		0);

	return PKT_PARSE_OK;
}


static ch_packet_parser_t udp_parser = {

	{NULL,NULL},
	CH_IPPROTO_UDP,
	_packet_udp_parse
};

void ch_packet_udp_init(void){


	ch_packet_parser_register(&udp_parser,L4_INDEX);
}

