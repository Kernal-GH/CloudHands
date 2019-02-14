/*
 *
 *      Filename: ch_packet_tcp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-13 11:31:10
 * Last Modified: 2018-04-09 11:32:21
 */

#include <rte_tcp.h>
#include "ch_packet_tcp.h"
#include "ch_packet.h"
#include "ch_log.h"
#include "ch_jhash.h"
#define TH_OFF(th)	(((th)->data_off & 0xf0) >> 4)

static int _packet_tcp_parse(ch_packet_t *pkt){

	const struct ipv4_hdr *iph;
	struct ipv4_hdr iph_copy;
	const struct tcp_hdr *th;
	struct tcp_hdr th_copy;
	
	struct rte_mbuf *mbuf = pkt->mbuf;

	iph = rte_pktmbuf_read(mbuf, pkt->l2_len, sizeof(*iph),&iph_copy);
	th = rte_pktmbuf_read(mbuf,pkt->parse_off, sizeof(*th), &th_copy);

	pkt->pkt_type = PKT_TYPE_TCP;

	if (unlikely(th == NULL)){
	
		ch_log(CH_LOG_ERR,"Invalid tcp packet!");
		return PKT_PARSE_DROP;
	}

	if(TH_OFF(th)<sizeof(*th)/4){
	
		ch_log(CH_LOG_DEBUG,"Invalid tcp packet!");
		return PKT_PARSE_DROP;
	}

	pkt->l4_len = TH_OFF(th)*4;
	pkt->parse_off += pkt->l4_len;

	pkt->hash = ch_jhash_4words_sort(iph->src_addr,
		iph->dst_addr,
		rte_be_to_cpu_16(th->src_port),
		rte_be_to_cpu_16(th->dst_port),
		0);

	return PKT_PARSE_OK;
}


static ch_packet_parser_t tcp_parser = {

	{NULL,NULL},
	CH_IPPROTO_TCP,
	_packet_tcp_parse
};

void ch_packet_tcp_init(void){


	ch_packet_parser_register(&tcp_parser,L4_INDEX);
}
