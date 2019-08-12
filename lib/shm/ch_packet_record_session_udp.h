/*
 *
 *      Filename: ch_packet_record_session_udp.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-16 09:43:41
 * Last Modified: 2018-04-16 09:43:41
 */

#ifndef CH_PACKET_RECORD_SESSION_UDP_H
#define CH_PACKET_RECORD_SESSION_UDP_H

typedef struct ch_packet_record_session_udp_t ch_packet_record_session_udp_t;

#include "ch_packet_record.h"

struct ch_packet_record_session_udp_t {

	uint8_t  is_timeout;

	uint16_t timeout_tv;
    uint16_t src_port;
    uint16_t dst_port;

    uint8_t  is_ipv6;
    uint32_t src_ip;
    uint32_t dst_ip;

    uint8_t *src_addr;
    uint8_t *dst_addr;

	uint64_t session_id;
	uint64_t req_packets;
	uint64_t res_packets;
	uint64_t req_bytes;
	uint64_t res_bytes;
	uint64_t req_start_time;
	uint64_t req_last_time;
	uint64_t res_start_time;
	uint64_t res_last_time;

	void *req_data;
	size_t req_dsize;

	void *res_data;
	size_t res_dsize;
};

#define CH_PACKET_RECORD_SESSION_UDP_META_SIZE(req_dsize,res_dsize) (sizeof(uint8_t)*2+\
	sizeof(uint16_t)*3+\
	sizeof(uint32_t)*2+\
	sizeof(uint64_t)*9+\
	sizeof(size_t)*4+\
	req_dsize+res_dsize+32)

extern void ch_packet_record_session_udp_read(ch_packet_record_session_udp_t *pkt_udp_session,ch_packet_record_t *pkt_rcd);

extern size_t 
ch_packet_record_session_udp_write(ch_packet_record_session_udp_t *pkt_udp_session,void *buffer,size_t bsize,void *data,size_t dlen);


#endif /*CH_PACKET_RECORD_SESSION_UDP_H*/
