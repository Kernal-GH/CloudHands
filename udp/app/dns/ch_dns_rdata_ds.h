/*
 *
 *      Filename: ch_dns_rdata_ds.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_DS_H
#define CH_DNS_RDATA_DS_H

typedef struct ch_dns_rdata_ds_t ch_dns_rdata_ds_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

struct ch_dns_rdata_ds_t {

	ch_dns_rdata_t rdata;

	uint16_t footprint;
	uint8_t alg;
	uint8_t digestid;

	uint16_t digest_len;
	unsigned char *digest;
};


extern void ch_dns_rdata_ds_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_DS_H*/
