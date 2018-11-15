/*
 *
 *      Filename: ch_dns_rdata_tlsa.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_TLSA_H
#define CH_DNS_RDATA_TLSA_H

typedef struct ch_dns_rdata_tlsa_t ch_dns_rdata_tlsa_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"


struct ch_dns_rdata_tlsa_t {

	ch_dns_rdata_t rdata;

	uint8_t cert_usage;
	uint8_t selector;
	uint8_t matching_type;

	uint16_t cert_dlen;
	unsigned char *cert_data;
};


extern void ch_dns_rdata_tlsa_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_TLSA_H*/
