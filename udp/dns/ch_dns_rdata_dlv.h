/*
 *
 *      Filename: ch_dns_rdata_dlv.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_DLV_H
#define CH_DNS_RDATA_DLV_H

typedef struct ch_dns_rdata_dlv_t ch_dns_rdata_dlv_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

struct ch_dns_rdata_dlv_t {

	ch_dns_rdata_t rdata;

	uint16_t footprint;
	uint8_t alg;
	uint8_t digestid;

	uint16_t digest_len;
	unsigned char *digest;
};


extern void ch_dns_rdata_dlv_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_DLV_H*/
