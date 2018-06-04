/*
 *
 *      Filename: ch_dns_header.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-07 15:05:40
 * Last Modified: 2018-05-07 15:18:41
 */

#include "ch_dns_header.h"
#include "ch_log.h"

int ch_dns_header_parse(ch_dns_data_input_t *din,ch_dns_header_t *hdr){

	uint8_t rcode;

	if(ch_dns_data_input_rdlen(din)<12){
	
		ch_log(CH_LOG_ERR,"Invalid DNS Packet!");
		return -1;
	}


	hdr->id = ch_dns_data_input_uint16_read(din);
	hdr->flags = ch_dns_data_input_uint16_read(din);
	
	rcode = ch_dns_hdr_rcode(hdr);
	if(rcode>5){
	
		ch_log(CH_LOG_ERR,"Invalid DNS Packet!");
		return -1;
	}

	hdr->qcount = ch_dns_data_input_uint16_read(din);
	hdr->ancount = ch_dns_data_input_uint16_read(din);
	hdr->aucount = ch_dns_data_input_uint16_read(din);
	hdr->adcount = ch_dns_data_input_uint16_read(din);


	return 0;
}


