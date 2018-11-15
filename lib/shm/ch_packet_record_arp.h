/*
 *
 *      Filename: ch_packet_record_arp.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-11 15:13:54
 * Last Modified: 2018-04-11 15:13:54
 */

#ifndef CH_PACKET_RECORD_ARP_H
#define CH_PACKET_RECORD_ARP_H

typedef struct ch_packet_record_arp_t ch_packet_record_arp_t;
typedef struct ch_arp_ipv4_t ch_arp_ipv4_t;
typedef struct ch_arp_addr_t ch_arp_addr_t;

#include "ch_packet_record.h"
#include "ch_net_util.h"

struct ch_arp_addr_t {

	uint8_t addr[6];
};

struct ch_arp_ipv4_t {

	uint32_t arp_sip; /*< sender IP address*/
	uint32_t arp_tip; /*< target IP address*/
	
	ch_arp_addr_t  arp_sha; /*< sender hardware address*/
	ch_arp_addr_t  arp_tha; /*< target hardware address*/

};

struct ch_packet_record_arp_t {

	uint16_t arp_hrd;    /* format of hardware address */

	uint16_t arp_pro;    /* format of protocol address */
	uint8_t  arp_hln;    /* length of hardware address */
	uint8_t  arp_pln;    /* length of protocol address */
	uint16_t arp_op;     /* ARP opcode (command) */



	ch_arp_ipv4_t arp_data;
	
	ch_arp_addr_t src_mac;
	ch_arp_addr_t dst_mac;
};

#define CH_PACKET_RECORD_ARP_META_SIZE (sizeof(uint8_t)*26+\
	sizeof(uint16_t)*3+\
	sizeof(uint32_t)*2)


extern void ch_packet_record_arp_read(ch_packet_record_arp_t *arp_rcd,ch_packet_record_t *pkt_rcd);

extern size_t ch_packet_record_arp_write(ch_packet_record_arp_t *arp_rcd,void *buffer,size_t bsize,void *data,size_t dlen);

static inline  void ch_packet_record_arp_dump(ch_packet_record_arp_t *arp_rcd,FILE *out){

	char buffer[64] ={0};

	fprintf(out,"Dump The ARP Packet Informations:\n");
	fprintf(out,"format of hardware address:%d\n",(int)arp_rcd->arp_hrd);
	fprintf(out,"format of protocol address:%d\n",(int)arp_rcd->arp_pro);
	fprintf(out,"length of hardware address:%d\n",(int)arp_rcd->arp_hln);
	fprintf(out,"length of protocol address:%d\n",(int)arp_rcd->arp_pln);
	fprintf(out,"ARP opcode (command):%d\n",(int)arp_rcd->arp_op);

	ch_mac_addr_format(buffer,64,arp_rcd->arp_data.arp_sha.addr);
	fprintf(out,"sender hardware address:%s\n",buffer);

	ch_mac_addr_format(buffer,64,arp_rcd->arp_data.arp_tha.addr);
	fprintf(out,"target hardware address:%s\n",buffer);

	ch_ip_to_str(buffer,64,arp_rcd->arp_data.arp_sip);
	fprintf(out,"sender ip address:%s\n",buffer);

	ch_ip_to_str(buffer,64,arp_rcd->arp_data.arp_tip);
	fprintf(out,"target ip address:%s\n",buffer);

	ch_mac_addr_format(buffer,64,arp_rcd->src_mac.addr);
	fprintf(out,"src mac address:%s\n",buffer);

	ch_mac_addr_format(buffer,64,arp_rcd->dst_mac.addr);
	fprintf(out,"dst mac address:%s\n",buffer);
}

#endif /*CH_PACKET_RECORD_ARP_H*/
