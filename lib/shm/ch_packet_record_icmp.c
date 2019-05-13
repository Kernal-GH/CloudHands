/*
 *
 *      Filename: ch_packet_record_icmp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-12 15:00:47
 * Last Modified: 2018-04-12 16:32:23
 */

#include "ch_packet_record_icmp.h"


void ch_packet_record_icmp_read(ch_packet_record_icmp_t *icmp_rcd,ch_packet_record_t *pkt_rcd){

	ch_bin_format_t btmp,*bfmt=&btmp;

	ch_bf_init_with_len(bfmt,pkt_rcd->shm_rcd.data,pkt_rcd->meta_data_size);

    icmp_rcd->icmp_type = ch_bf_uint8_read(bfmt);
    icmp_rcd->icmp_code = ch_bf_uint8_read(bfmt);
    icmp_rcd->icmp_ttl = ch_bf_uint8_read(bfmt);
    icmp_rcd->icmp_cksum = ch_bf_uint16_read(bfmt);
    icmp_rcd->icmp_ident = ch_bf_uint16_read(bfmt);
	icmp_rcd->icmp_seq_nb = ch_bf_uint16_read(bfmt);

	icmp_rcd->icmp_sip = ch_bf_uint32_read(bfmt);
	icmp_rcd->icmp_tip = ch_bf_uint32_read(bfmt);

}

size_t ch_packet_record_icmp_write(ch_packet_record_icmp_t *icmp_rcd,void *buffer,size_t bsize,void *data,size_t dlen){

	ch_bin_format_t btmp,*bfmt=&btmp;

	ch_bf_init_with_len(bfmt,buffer,bsize);

    ch_bf_uint8_write(bfmt,icmp_rcd->icmp_type);
    ch_bf_uint8_write(bfmt,icmp_rcd->icmp_code);
    ch_bf_uint8_write(bfmt,icmp_rcd->icmp_ttl);
    ch_bf_uint16_write(bfmt,icmp_rcd->icmp_cksum);
    ch_bf_uint16_write(bfmt,icmp_rcd->icmp_ident);
	ch_bf_uint16_write(bfmt,icmp_rcd->icmp_seq_nb);
	ch_bf_uint32_write(bfmt,icmp_rcd->icmp_sip);
	ch_bf_uint32_write(bfmt,icmp_rcd->icmp_tip);

	if(data&&dlen>0){
	
		ch_bf_bytes_write(bfmt,data,dlen);
    }else{
		ch_bf_bytes_write(bfmt,"N",1);
    }

	return ch_bf_content_size(bfmt);

}
