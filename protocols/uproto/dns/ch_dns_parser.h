/*
 *
 *      Filename: ch_dns_parser.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-31 00:41:32
 * Last Modified: 2016-10-31 00:41:32
 */

#ifndef CH_DNS_PARSER_H
#define CH_DNS_PARSER_H

#include <apr_pools.h>
#include "ch_dns_session.h"

extern int ch_dns_hdr_parse(ch_dns_hdr_t *dns_hdr,const unsigned char *packet,uint32_t pos,uint32_t len);

extern int ch_dns_question_parse(apr_pool_t *mp,ch_dns_question_t *dns_question,const unsigned char * packet,
	uint32_t *packet_p,uint32_t id_pos,uint32_t len);


extern int ch_dns_answer_parse(apr_pool_t *mp,ch_dns_answer_t *ans,const unsigned char * packet,
	uint32_t *packet_p,uint32_t id_pos,uint32_t len);

#endif /* CH_DNS_PARSER_H */
