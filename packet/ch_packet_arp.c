/*
 *
 *      Filename: ch_packet_arp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-11 17:51:47
 * Last Modified: 2018-04-11 19:01:09
 */

#include "ch_packet_arp.h"
#include <rte_arp.h>
#include "ch_log.h"
#include "ch_jhash.h"

static int _packet_arp_parse(ch_packet_t *pkt){

	const struct arp_hdr *arp;
	struct arp_hdr arp_copy;
	struct rte_mbuf *mbuf = pkt->mbuf;
	uint16_t off = pkt->parse_off;

	
	arp = rte_pktmbuf_read(mbuf, off, sizeof(*arp), &arp_copy);

	if (unlikely(arp == NULL)){
		
		ch_log(CH_LOG_DEBUG,"Invalid arp packet!");

		return PKT_PARSE_DROP;
	}
    
	pkt->pkt_type = PKT_TYPE_ARP;
	
	pkt->l3_len = sizeof(*arp);

	pkt->parse_off += pkt->l3_len;


	pkt->hash = ch_jhash_4words_sort(arp->arp_data.arp_sip,
		arp->arp_data.arp_tip,
		0,
		0,
		0);
	
	return PKT_PARSE_OK;

}

static ch_packet_parser_t arp_parser = {

	{NULL,NULL},
	CH_ETH_P_ARP,
	_packet_arp_parse,
};

void ch_packet_arp_init(void){

	ch_packet_parser_register(&arp_parser,L3_INDEX);

}
