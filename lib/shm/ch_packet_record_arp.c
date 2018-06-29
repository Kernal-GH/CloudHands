/*
 *
 *      Filename: ch_packet_record_arp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-11 16:18:35
 * Last Modified: 2018-06-29 11:33:30
 */

#include "ch_packet_record_arp.h"

#define addr_write(bfmt,arp_addr) do { \
    ch_bf_uint8_write(bfmt,arp_addr->addr[0]);\
    ch_bf_uint8_write(bfmt,arp_addr->addr[1]);\
    ch_bf_uint8_write(bfmt,arp_addr->addr[2]);\
    ch_bf_uint8_write(bfmt,arp_addr->addr[3]);\
    ch_bf_uint8_write(bfmt,arp_addr->addr[4]);\
    ch_bf_uint8_write(bfmt,arp_addr->addr[5]);\
}while(0)

#define addr_read(bfmt,arp_addr) do { \
  arp_addr->addr[0]=ch_bf_uint8_read(bfmt);\
  arp_addr->addr[1]=ch_bf_uint8_read(bfmt);\
  arp_addr->addr[2]=ch_bf_uint8_read(bfmt);\
  arp_addr->addr[3]=ch_bf_uint8_read(bfmt);\
  arp_addr->addr[4]=ch_bf_uint8_read(bfmt);\
  arp_addr->addr[5]=ch_bf_uint8_read(bfmt);\
}while(0)

void ch_packet_record_arp_read(ch_packet_record_arp_t *arp_rcd,ch_packet_record_t *pkt_rcd){

	ch_arp_addr_t *arp_sha = &(arp_rcd->arp_data.arp_sha);
	ch_arp_addr_t *arp_tha = &(arp_rcd->arp_data.arp_tha);
	ch_arp_addr_t *src_mac = &arp_rcd->src_mac;
	ch_arp_addr_t *dst_mac = &arp_rcd->dst_mac;

	ch_bin_format_t btmp,*bfmt=&btmp;

	ch_bf_init_with_len(bfmt,pkt_rcd->shm_rcd.data,pkt_rcd->meta_data_size);

    arp_rcd->arp_hrd = ch_bf_uint16_read(bfmt);
    arp_rcd->arp_pro = ch_bf_uint16_read(bfmt);
    arp_rcd->arp_hln = ch_bf_uint8_read(bfmt);
    arp_rcd->arp_pln = ch_bf_uint8_read(bfmt);
    
	arp_rcd->arp_op = ch_bf_uint16_read(bfmt);

    arp_rcd->arp_data.arp_sip = ch_bf_uint32_read(bfmt);
	arp_rcd->arp_data.arp_tip = ch_bf_uint32_read(bfmt);

	addr_read(bfmt,arp_sha);
	addr_read(bfmt,arp_tha);
	addr_read(bfmt,src_mac);
	addr_read(bfmt,dst_mac);

}

size_t ch_packet_record_arp_write(ch_packet_record_arp_t *arp_rcd,void *buffer,size_t bsize,void *data,size_t dlen){

	ch_arp_addr_t *arp_sha = &(arp_rcd->arp_data.arp_sha);
	ch_arp_addr_t *arp_tha = &(arp_rcd->arp_data.arp_tha);
	ch_arp_addr_t *src_mac = &arp_rcd->src_mac;
	ch_arp_addr_t *dst_mac = &arp_rcd->dst_mac;
	
	ch_bin_format_t btmp,*bfmt=&btmp;

	ch_bf_init_with_len(bfmt,buffer,bsize);
    ch_bf_uint16_write(bfmt,arp_rcd->arp_hrd);
    ch_bf_uint16_write(bfmt,arp_rcd->arp_pro);
    ch_bf_uint8_write(bfmt,arp_rcd->arp_hln);
    ch_bf_uint8_write(bfmt,arp_rcd->arp_pln);
    
	ch_bf_uint16_write(bfmt,arp_rcd->arp_op);

    ch_bf_uint32_write(bfmt,arp_rcd->arp_data.arp_sip);
	ch_bf_uint32_write(bfmt,arp_rcd->arp_data.arp_tip);

	addr_write(bfmt,arp_sha);
	addr_write(bfmt,arp_tha);
	addr_write(bfmt,src_mac);
	addr_write(bfmt,dst_mac);

	if(data&&dlen>0){
	
		ch_bf_bytes_write(bfmt,data,dlen);
	}

	return ch_bf_content_size(bfmt);

}
