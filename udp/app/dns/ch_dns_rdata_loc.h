/*
 *
 *      Filename: ch_dns_rdata_loc.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_LOC_H
#define CH_DNS_RDATA_LOC_H

typedef struct ch_dns_rdata_loc_t ch_dns_rdata_loc_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

struct ch_dns_rdata_loc_t {

	ch_dns_rdata_t rdata;

	uint8_t version;
	uint8_t size;
	uint8_t hprecision;
	uint8_t vprecision;
	uint32_t latitude;
	uint32_t longitude;
	uint32_t altitude;
};


extern void ch_dns_rdata_loc_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_LOC_H*/
