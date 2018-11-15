/*
 *
 *      Filename: ch_dns_rdata_wks.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-04 16:23:03
 * Last Modified: 2018-05-04 16:23:03
 */

#ifndef CH_DNS_RDATA_WKS_H
#define CH_DNS_RDATA_WKS_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

typedef struct ch_dns_rdata_wks_t ch_dns_rdata_wks_t;

struct ch_dns_rdata_wks_t {

	ch_dns_rdata_t rdata;

	uint32_t address;
	uint8_t protocol;

	uint16_t map_len;

	unsigned char *bitmap;
};


extern void ch_dns_rdata_wks_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_WKS_H*/
