/*
 *
 *      Filename: ch_packet_record.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-10 09:46:18
 * Last Modified: 2018-04-10 09:46:18
 */

#ifndef CH_PACKET_RECORD_H
#define CH_PACKET_RECORD_H

typedef struct ch_packet_record_t ch_packet_record_t;

#include "ch_shm_format.h"

/*Define record types*/
#define PKT_RECORD_TYPE_SESSION_TCP 1
#define PKT_RECORD_TYPE_SESSION_UDP 2
#define PKT_RECORD_TYPE_ICMP 3
#define PKT_RECORD_TYPE_ARP 4
#define PKT_RECORD_TYPE_OTHER 5

/*Define udp app record types*/
#define PKT_RECORD_TYPE_UDP_DNS 6
#define PKT_RECORD_TYPE_UDP_TFTP 7
#define PKT_RECORD_TYPE_UDP_SMON 8

/*Define tcp app record types*/
#define PKT_RECORD_TYPE_TCP_HTTP 9
#define PKT_RECORD_TYPE_TCP_MAIL 10
#define PKT_RECORD_TYPE_TCP_FTP  11
#define PKT_RECORD_TYPE_TCP_TELNET 12
#define PKT_RECORD_TYPE_TCP_SMON 13
#define PKT_RECORD_TYPE_MREQ_HTTP 14
#define PKT_RECORD_TYPE_MREQ_MAIL 15
#define PKT_RECORD_TYPE_TCP_FTP_DATA 16
#define PKT_RECORD_TYPE_TCP_SSH 17

/*Define some stat record types*/
#define PKT_RECORD_TYPE_STAT_FTP 18
#define PKT_RECORD_TYPE_STAT_SSH 19
#define PKT_RECORD_TYPE_STAT_TELNET 20

struct ch_packet_record_t {

	ch_shm_record_t shm_rcd;

	uint8_t type;
	uint16_t meta_data_size;
	
	uint64_t time;

};

#define CH_PKT_RECORD_HEADER_SIZE (sizeof(uint8_t)+\
	sizeof(uint32_t)*3+\
	sizeof(size_t)+\
	sizeof(uint16_t)+\
	sizeof(uint64_t))

extern ch_shm_format_t * ch_shm_format_pkt_with_mmap_create(ch_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,
	uint64_t priv_data_size,int is_write);


extern ch_shm_format_t * ch_shm_format_pkt_with_shm_create(ch_pool_t *mp,const char *key,int proj_id,uint64_t size,
	uint64_t entry_size,uint64_t priv_data_size,int is_write);

extern int ch_packet_record_put(ch_shm_format_t *shm_fmt,ch_packet_record_t *pkt_rcd,void *data,size_t dlen);


#endif /*CH_PACKET_RECORD_H*/
