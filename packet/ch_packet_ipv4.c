/*
 *
 *      Filename: ch_packet_ipv4.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-13 10:20:18
 * Last Modified: 2018-04-21 17:26:27
 */

#include "ch_packet_ipv4.h"
#include "ch_packet.h"
#include <rte_ip.h>

#include "ch_log.h"

#define IPV(hdr) ((((hdr)->version_ihl)&0xf0)>>4)
#define IPHL(hdr) (((hdr)->version_ihl)&0x0f)

static int _packet_ipv4_parse(ch_packet_t *pkt){

	const struct ipv4_hdr *ip4h;
	struct ipv4_hdr ip4h_copy;
	struct rte_mbuf *mbuf = pkt->mbuf;
	uint16_t off = pkt->parse_off;

	ip4h = rte_pktmbuf_read(mbuf, off, sizeof(*ip4h), &ip4h_copy);

	if (unlikely(ip4h == NULL)){
		
		ch_log(CH_LOG_DEBUG,"Invalid ipv4 packet!");

		return PKT_PARSE_DROP;
	}
    
	/*check ip header len,and version */
    if(IPHL(ip4h)<5||IPV(ip4h)!=4){
        ch_log(CH_LOG_DEBUG,"invalid ip header!");
        return PKT_PARSE_DROP;
    }

	pkt->l3_len = IPHL(ip4h)*4;

	pkt->parse_off += pkt->l3_len;

	if (ip4h->fragment_offset & rte_cpu_to_be_16(
			IPV4_HDR_OFFSET_MASK | IPV4_HDR_MF_FLAG)) {
		
		pkt->l4_len = 0;
		pkt->l4_proto = 0;

		return PKT_PARSE_BREAK;
	}
    


	pkt->l4_proto = ip4h->next_proto_id;
	
	if(pkt->l4_proto<=CH_IPPROTO_IP||pkt->l4_proto>=CH_IPPROTO_MAX){
     
		ch_log(CH_LOG_ERR,"UnImplemented next protocol :%d",(int)pkt->l4_proto);

		return PKT_PARSE_DROP;
	}

	return PKT_PARSE_OK;
}

static ch_packet_parser_t ipv4_parser = {

	{NULL,NULL},
	CH_ETH_P_IP,
	_packet_ipv4_parse,
};

void ch_packet_ipv4_init(void){

	ch_packet_parser_register(&ipv4_parser,L3_INDEX);

}
