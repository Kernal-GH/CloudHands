/*
 *
 *      Filename: ch_dns_rdata_sshfp.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_SSHFP_H
#define CH_DNS_RDATA_SSHFP_H

typedef struct ch_dns_rdata_sshfp_t ch_dns_rdata_sshfp_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

struct ch_dns_rdata_sshfp_t {

	ch_dns_rdata_t rdata;

	uint8_t alg;
	uint8_t digest_type;

	uint16_t fp_len;
	unsigned char *fingerprint;

};


extern void ch_dns_rdata_sshfp_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_SSHFP_H*/
