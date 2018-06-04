/*
 *
 *      Filename: ch_dns_rdata_ipv6.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-04 17:20:37
 * Last Modified: 2018-05-04 17:20:37
 */

#ifndef CH_DNS_RDATA_IPV6_H
#define CH_DNS_RDATA_IPV6_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

typedef struct ch_dns_rdata_ipv6_t ch_dns_rdata_ipv6_t;


struct ch_dns_rdata_ipv6_t {

	ch_dns_rdata_t rdata;

	unsigned char *address;

};

extern void ch_dns_rdata_ipv6_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_IPV6_H*/
