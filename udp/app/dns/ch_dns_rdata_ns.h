/*
 *
 *      Filename: ch_dns_rdata_ns.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:01:24
 * Last Modified: 2018-05-03 19:01:24
 */

#ifndef CH_DNS_RDATA_NS_H
#define CH_DNS_RDATA_NS_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_ns_t ch_dns_rdata_ns_t;


struct ch_dns_rdata_ns_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t name;

};

extern void ch_dns_rdata_ns_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_NS_H*/
