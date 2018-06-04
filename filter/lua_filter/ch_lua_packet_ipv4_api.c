/*
 *
 *      Filename: ch_lua_packet_ipv4_api.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-14 17:19:22
 * Last Modified: 2017-12-14 17:33:17
 */


#include <rte_ip.h>
#include "ch_packet.h"
#include "ch_lua_packet_api.h"

typedef struct {
	
	uint8_t  vihl;	
	uint8_t  tos;	
	uint16_t tlen;		
	uint16_t pid;		
	uint16_t fragOff;
	uint8_t  ttl;
	uint8_t  protoID;
	uint16_t checksum;
	uint32_t srcIP;
	uint32_t dstIP;

} ch_packet_ipv4_t;

int ch_packet_ipv4_hdr_get(ch_packet_t *pkt,ch_packet_ipv4_t *ipv4){

	const struct ipv4_hdr *ip4h;
	struct ipv4_hdr ip4h_copy;

	ip4h = rte_pktmbuf_read(pkt->mbuf, pkt->l2_len, sizeof(*ip4h),&ip4h_copy);

	if(unlikely(ip4h == NULL))
		return LUA_FFI_ERROR;


	ipv4->vihl = ip4h->version_ihl;
	ipv4->tos = ip4h->type_of_service;
	ipv4->tlen = ip4h->total_length;
	ipv4->pid = ip4h->packet_id;
	ipv4->fragOff = ip4h->fragment_offset;
	ipv4->ttl = ip4h->time_to_live;
	ipv4->protoID = ip4h->next_proto_id;
	ipv4->checksum = ip4h->hdr_checksum;
	ipv4->srcIP = ip4h->src_addr;
	ipv4->dstIP = ip4h->dst_addr;


	return LUA_FFI_OK;
}


