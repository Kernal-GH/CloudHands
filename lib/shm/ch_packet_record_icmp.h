/*
 *
 *      Filename: ch_packet_record_icmp.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-12 12:18:14
 * Last Modified: 2018-04-12 12:18:14
 */

#ifndef CH_PACKET_RECORD_ICMP_H
#define CH_PACKET_RECORD_ICMP_H

typedef struct ch_packet_record_icmp_t ch_packet_record_icmp_t;

#include "ch_packet_record.h"
#include "ch_net_util.h"

struct ch_packet_record_icmp_t {

	uint8_t  icmp_type;   /* ICMP packet type. */
	uint8_t  icmp_code;   /* ICMP packet code. */
	uint8_t  icmp_ttl;    /* ICMP packet time to live*/
	uint16_t icmp_cksum;  /* ICMP packet checksum. */
	uint16_t icmp_ident;  /* ICMP packet identifier. */
	uint16_t icmp_seq_nb; /* ICMP packet sequence number. */

	uint32_t icmp_sip;    /*ICMP packet send ip*/
	uint32_t icmp_tip;    /*ICMP packet target ip*/

    size_t dlen;
    void *data;
};
#define CH_PACKET_RECORD_ICMP_META_SIZE (sizeof(uint8_t)*3+\
	sizeof(uint16_t)*3+\
	sizeof(uint32_t)*2)


extern void ch_packet_record_icmp_read(ch_packet_record_icmp_t *icmp_rcd,ch_packet_record_t *pkt_rcd);

extern size_t ch_packet_record_icmp_write(ch_packet_record_icmp_t *icmp_rcd,void *buffer,size_t bsize,void *data,size_t dlen);

static inline  void ch_packet_record_icmp_dump(ch_packet_record_icmp_t *icmp_rcd,FILE *out){

	char buffer[64] = {0};

	fprintf(out,"Dump The ICMP Packet Informations:\n");
	fprintf(out,"ICMP packet type:%d\n",(int)icmp_rcd->icmp_type);
	fprintf(out,"ICMP packet code:%d\n",(int)icmp_rcd->icmp_code);
	fprintf(out,"ICMP packet TTL:%d\n",(int)icmp_rcd->icmp_ttl);
	fprintf(out,"ICMP packet checksum:%d\n",(int)icmp_rcd->icmp_cksum);
	fprintf(out,"ICMP packet identifier:%d\n",(int)icmp_rcd->icmp_ident);
	fprintf(out,"ICMP packet sequence number:%d\n",(int)icmp_rcd->icmp_seq_nb);
	
	ch_ip_to_str(buffer,64,icmp_rcd->icmp_sip);
	fprintf(out,"ICMP packet send ip:%s\n",buffer);

	ch_ip_to_str(buffer,64,icmp_rcd->icmp_tip);
	fprintf(out,"ICMP packet target ip:%s\n",buffer);

}
#endif /*CH_PACKET_RECORD_ICMP_H*/
