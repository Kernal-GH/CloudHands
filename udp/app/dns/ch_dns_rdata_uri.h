/*
 *
 *      Filename: ch_dns_rdata_uri.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:08:57
 * Last Modified: 2018-05-03 19:08:57
 */

#ifndef CH_DNS_RDATA_URI_H
#define CH_DNS_RDATA_URI_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_uri_t ch_dns_rdata_uri_t;


struct ch_dns_rdata_uri_t {

	ch_dns_rdata_t rdata;

	uint16_t priority;
	uint16_t weight;

	uint16_t target_len;
	unsigned char *target;

};

extern void ch_dns_rdata_uri_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_URI_H*/
