/*
 *
 *      Filename: ch_packet_icmp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-12 12:10:14
 * Last Modified: 2018-04-21 17:25:18
 */

#include "ch_packet_icmp.h"

#include "ch_log.h"
#include "ch_jhash.h"

static int _packet_icmp_parse(ch_packet_t *pkt){

	const struct ipv4_hdr *iph;
	struct ipv4_hdr iph_copy;

	struct rte_mbuf *mbuf = pkt->mbuf;

	iph = rte_pktmbuf_read(mbuf, pkt->l2_len, sizeof(*iph),&iph_copy);
	if(iph == NULL)
		return PKT_PARSE_DROP;

	pkt->pkt_type = PKT_TYPE_ICMP;

	pkt->l4_len = sizeof(struct icmp_hdr);

	pkt->parse_off += pkt->l4_len;

	pkt->hash = ch_jhash_4words_sort(iph->src_addr,
		iph->dst_addr,
		0,
		0,
		0);

	return PKT_PARSE_OK;
}


static ch_packet_parser_t icmp_parser = {

	{NULL,NULL},
	CH_IPPROTO_ICMP,
	_packet_icmp_parse
};

void ch_packet_icmp_init(void){


	ch_packet_parser_register(&icmp_parser,L4_INDEX);
}

