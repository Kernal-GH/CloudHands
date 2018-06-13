/*
 *
 *      Filename: ch_dns_rdata_nsap_ptr.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 15:51:23
 * Last Modified: 2018-05-03 15:51:23
 */

#ifndef CH_DNS_RDATA_NSAP_PTR_H
#define CH_DNS_RDATA_NSAP_PTR_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_nsap_ptr_t ch_dns_rdata_nsap_ptr_t;


struct ch_dns_rdata_nsap_ptr_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t name;

};

extern void ch_dns_rdata_nsap_ptr_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_NSAP_PTR_H*/
