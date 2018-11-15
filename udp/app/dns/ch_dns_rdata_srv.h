/*
 *
 *      Filename: ch_dns_rdata_srv.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:08:57
 * Last Modified: 2018-05-03 19:08:57
 */

#ifndef CH_DNS_RDATA_SRV_H
#define CH_DNS_RDATA_SRV_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_srv_t ch_dns_rdata_srv_t;


struct ch_dns_rdata_srv_t {

	ch_dns_rdata_t rdata;

	uint16_t priority;
	uint16_t weight;
	uint16_t port;

	ch_dns_name_t target; 
};

extern void ch_dns_rdata_srv_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_SRV_H*/
