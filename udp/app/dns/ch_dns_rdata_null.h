/*
 *
 *      Filename: ch_dns_rdata_null.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 18:38:34
 * Last Modified: 2018-05-03 18:38:34
 */

#ifndef CH_DNS_RDATA_NULL_H
#define CH_DNS_RDATA_NULL_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

typedef struct ch_dns_rdata_null_t ch_dns_rdata_null_t;


struct ch_dns_rdata_null_t {

	ch_dns_rdata_t rdata;

	uint16_t dlen;

	unsigned char *data;
};

extern void ch_dns_rdata_null_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_NULL_H*/
