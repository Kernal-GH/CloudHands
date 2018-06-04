/*
 *
 *      Filename: ch_dns_parser.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-31 00:48:04
 * Last Modified: 2016-11-01 21:27:18
 */

#include "ch_string.h"
#include "ch_log.h"
#include "ch_dns_parser.h"

int ch_dns_hdr_parse(ch_dns_hdr_t *dns_hdr,const unsigned char *packet,uint32_t pos,uint32_t len){

	/* no enough data, */
	if(len - pos <12)
		return -1;

	/* dns identifier */
    dns_hdr->id = (packet[pos] << 8) + packet[pos+1];
    dns_hdr->qr = packet[pos+2] >> 7;
    dns_hdr->AA = (packet[pos+2] & 0x04) >> 2;
    dns_hdr->TC = (packet[pos+2] & 0x02) >> 1;
    dns_hdr->rcode = packet[pos + 3] & 0x0f;

    // rcodes > 5 indicate various protocol errors and redefine most of the 
    // remaining fields. Parsing this would hurt more than help. 
    if (dns_hdr->rcode > 5) {
		ch_log(CH_LOG_ERR,"Invalid dns packet!");
        return -1;
    }
	
	dns_hdr->flags = (packet[pos+2] << 8) + packet[pos+3];;

	/* dns questions count */
    dns_hdr->qcount = (packet[pos+4] << 8) + packet[pos+5];
	
	/* dns answers count */
	dns_hdr->ancount = (packet[pos+6] << 8) + packet[pos+7];
    
	/* dns authorities count */
	dns_hdr->aucount = (packet[pos+8] << 8) + packet[pos+9];
    
	/* dns additionals count */
	dns_hdr->adcount = (packet[pos+10] << 8) + packet[pos+11];


	/* ok! */
	return 0;
}

int ch_dns_question_parse(apr_pool_t *mp,ch_dns_question_t *dns_question,const unsigned char * packet,
	uint32_t *packet_p,uint32_t id_pos,uint32_t len){

	uint32_t pos;
	dns_question->qname = ch_read_rr_name(mp,packet,packet_p,id_pos,len);

	pos = *packet_p;

	if(dns_question->qname == NULL ||(pos+4)>len){
	
		ch_log(CH_LOG_ERR,"NS question error!");
		return -1;
	}

	dns_question->qtype = (packet[pos] << 8) + packet[pos+1];
	dns_question->qclass = (packet[pos+2] << 8) + packet[pos+3];

	*packet_p = *packet_p+4;

	/* ok! */
	return 0;
}

int ch_dns_answer_parse(apr_pool_t *mp,ch_dns_answer_t *ans,const unsigned char *packet,
	uint32_t *packet_p,uint32_t id_pos,uint32_t len){

	int i;
	uint32_t pos;
	uint32_t rr_start = *packet_p;

	ch_dns_rdata_parse_t *parser ;
	ch_dns_rdata_parse_t opts_parser = {0,0,_rdata_opts_parse,0};

	ans->rname = NULL;
	ans->rdata = NULL;

	/* read rname */
	ans->rname = ch_read_rr_name(mp,packet,packet_p,id_pos,len);
	if(ans->rname == NULL){
	
		ch_log(CH_LOG_ERR,"A bad rr name!");
		return -1;
	}
	pos = *packet_p;
	if(len-pos<10){
	
		ch_log(CH_LOG_ERR,"The dns packet has been truncated!");
		return -1;
	}
    
	ans->rtype = (packet[pos] << 8) + packet[pos+1];
    ans->rdatalen = (packet[pos+8] << 8) + packet[pos + 9];
    
	// Handle dns opt RR's differently.
    if (ans->rtype == 41) {
        ans->rclass = 0;
        ans->rttl = 0;
        parser = &opts_parser;
        // We'll leave the parsing of the special EDNS opt fields to
        // our opt rdata parser.  
        pos = pos + 2;
    } else {
        // The normal case.
        ans->rclass = (packet[pos+2] << 8) + packet[pos+3];
        ans->rttl = 0;
        
		for (i=0; i<4; i++)
            ans->rttl = (ans->rttl << 8) + packet[pos+4+i];

        // Retrieve the correct parser function.
		parser = ch_dns_rdata_parser_find(ans->rclass,ans->rtype);
        pos = pos + 10;
    }

    // Make sure the data for the record is actually there.
    // If not, escape and print the raw data.
	if(len<rr_start+10+ans->rdatalen){
	
		ch_log(CH_LOG_ERR,"Rdata has been truncated,");
	
		return -1;
	}

	ans->rdata = parser->rdata_parse(mp,packet,pos,id_pos,ans->rdatalen,len);

	*packet_p = pos+ans->rdatalen;

	return 0;
}

