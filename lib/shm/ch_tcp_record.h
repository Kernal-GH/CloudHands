/*
 *
 *      Filename: ch_tcp_record.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-29 11:18:39
 * Last Modified: 2018-01-29 11:18:39
 */

#ifndef CH_TCP_RECORD_H
#define CH_TCP_RECORD_H

typedef struct ch_tcp_record_t ch_tcp_record_t;

#include "ch_shm_format.h"

struct ch_tcp_record_t {

	ch_shm_record_t shm_rcd;

    uint8_t  packet_type;
    uint8_t  session_direct;
    uint32_t protocol_id;
    uint64_t session_id;

    uint64_t time;
    
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;

	uint64_t req_packets;
	uint64_t req_bytes;
	
	uint64_t res_packets;
	uint64_t res_bytes;
};

#define CH_TCP_RECORD_HEADER_SIZE (sizeof(uint8_t)*2+sizeof(uint16_t)*2+sizeof(uint32_t)*6+sizeof(uint64_t)*6+sizeof(size_t))

extern ch_shm_format_t * ch_shm_format_tcp_with_mmap_create(ch_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,
	uint64_t priv_data_size,int is_write);


extern ch_shm_format_t * ch_shm_format_tcp_with_shm_create(ch_pool_t *mp,const char *key,int proj_id,uint64_t size,
	uint64_t entry_size,uint64_t priv_data_size,int is_write);


extern int ch_tcp_record_put(ch_shm_format_t *shm_fmt,ch_tcp_record_t *tcp_rcd);

#endif /*CH_TCP_RECORD_H*/
