/*************************************************************************
 > File Name: ch_test_util.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年05月10日 星期二 14时16分11秒
 ************************************************************************/

#include <rte_ip.h>
#include <rte_mbuf.h>
#include <rte_tcp.h>
#include <rte_ether.h>

#include "ch_test_util.h"
#include "ch_util.h"

static inline uint16_t _mbuf_hdr_length_get(){

	return sizeof(struct ether_hdr)+sizeof(struct ipv4_hdr)+sizeof(struct tcp_hdr);
}

static inline uint16_t _mbuf_length_get(uint16_t dlen){

	return sizeof(struct rte_mbuf)+sizeof(struct ether_hdr)+sizeof(struct ipv4_hdr)+sizeof(struct tcp_hdr)+dlen;

}

static inline void _mbuf_init(struct rte_mbuf *mbuf,uint32_t len,uint16_t dlen){

	mbuf->data_len = len;
	mbuf->pkt_len = len;
	mbuf->l2_len = sizeof(struct ether_hdr);
	mbuf->l3_len = sizeof(struct ipv4_hdr);
	mbuf->l4_len = sizeof(struct tcp_hdr);

}

static inline void _mbuf_ether_hdr_init(){

}

static inline void _mbuf_ipv4_hdr_init(struct rte_mbuf *mbuf,uint32_t src_ip,uint32_t dst_ip,uint16_t len){

	struct ipv4_hdr *iph = ch_ipv4_hdr_get(mbuf);
	iph->version_ihl = (4<<4)|(sizeof(struct ipv4_hdr)/4);
	iph->type_of_service = 0;
	iph->total_length = htons(len - sizeof(struct ether_hdr));
	iph->packet_id = 0;
	iph->fragment_offset = 0;
	iph->time_to_live = 4;
	iph->next_proto_id = 0;
	iph->hdr_checksum =  rte_ipv4_cksum(iph);
	iph->src_addr = htonl(src_ip);
	iph->dst_addr = htonl(dst_ip);
}

static inline void _mbuf_tcp_hdr_init(struct rte_mbuf *mbuf,
		uint16_t src_port,uint16_t dst_port,uint32_t seq,uint32_t seq_ack,uint8_t tcp_flags){

	struct tcp_hdr *th = ch_tcp_hdr_get(mbuf);
	th->src_port = htons(src_port);
	th->dst_port = htons(dst_port);
	th->sent_seq = htonl(seq);
	th->recv_ack = htonl(seq_ack);
	th->data_off = ((sizeof(struct tcp_hdr) / sizeof(uint32_t)) << 4);
	th->tcp_flags = tcp_flags;
	th->rx_win = 0;
	th->cksum = 0;
	th->tcp_urp = 0;
}

static inline void _mbuf_data_append(struct rte_mbuf *mbuf,void *data,uint16_t dlen){

	void *dst_addr = rte_pktmbuf_mtod(mbuf,void*)+_mbuf_hdr_length_get();
	memcpy(dst_addr,data,dlen);
}

struct rte_mbuf * alloc_a_mbuf(uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port,
		void *data,uint16_t dlen,uint32_t seq,uint32_t seq_ack,uint16_t tcp_flags){

	struct rte_mbuf * mbuf;
	uint32_t len = _mbuf_length_get(dlen);

	mbuf = (struct rte_mbuf*)calloc(1,len);
	if(!mbuf){
		return NULL;
	}

	_mbuf_init(mbuf,len,dlen);
	_mbuf_ether_hdr_init();
	_mbuf_ipv4_hdr_init(mbuf,src_ip,dst_ip,len);
	_mbuf_tcp_hdr_init(mbuf,src_port,dst_port,seq,seq_ack,tcp_flags);
	_mbuf_data_append(mbuf,data,dlen);

	return mbuf;
}

void free_a_mbuf(struct rte_mbuf *mbuf){
	free(mbuf);
}

