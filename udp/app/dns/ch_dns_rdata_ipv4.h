/*
 *
 *      Filename: ch_dns_rdata_ipv4.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-27 14:53:37
 * Last Modified: 2018-04-27 14:53:37
 */

#ifndef CH_DNS_RDATA_IPV4_H
#define CH_DNS_RDATA_IPV4_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

typedef struct ch_dns_rdata_ipv4_t ch_dns_rdata_ipv4_t;


struct ch_dns_rdata_ipv4_t {

	ch_dns_rdata_t rdata;

	uint32_t address;

};

extern void ch_dns_rdata_ipv4_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_IPV4_H*/
