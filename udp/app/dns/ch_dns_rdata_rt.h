/*
 *
 *      Filename: ch_dns_rdata_rt.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 16:50:20
 * Last Modified: 2018-05-03 16:50:20
 */

#ifndef CH_DNS_RDATA_RT_H
#define CH_DNS_RDATA_RT_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_rt_t ch_dns_rdata_rt_t;


struct ch_dns_rdata_rt_t {

	ch_dns_rdata_t rdata;
	
	uint16_t preference;

	ch_dns_name_t midhost;

};

extern void ch_dns_rdata_rt_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_RT_H*/
