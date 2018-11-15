/*
 *
 *      Filename: ch_dns_rdata_naptr.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 15:58:47
 * Last Modified: 2018-05-03 15:58:47
 */

#ifndef CH_DNS_RDATA_NAPTR_H
#define CH_DNS_RDATA_NAPTR_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_naptr_t ch_dns_rdata_naptr_t;


struct ch_dns_rdata_naptr_t {

	ch_dns_rdata_t rdata;
	
	uint16_t order;
	uint16_t preference;
	
	unsigned char *flags;
	unsigned char *service;
	unsigned char *regex;

	ch_dns_name_t replacement;

};

extern void ch_dns_rdata_naptr_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_NAPTR_H*/
