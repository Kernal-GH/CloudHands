/*
 *
 *      Filename: ch_dns_rdata_tsig.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_TSIG_H
#define CH_DNS_RDATA_TSIG_H

typedef struct ch_dns_rdata_tsig_t ch_dns_rdata_tsig_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"


struct ch_dns_rdata_tsig_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t alg;
	
	uint64_t timeSigned;
	uint16_t fudge;
	
	uint16_t sig_len;
	unsigned char *signature;

	uint16_t orig_id;
	uint16_t error;

	uint16_t other_len;
	unsigned char *other;
};


extern void ch_dns_rdata_tsig_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_TSIG_H*/
