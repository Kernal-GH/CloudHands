/*
 *
 *      Filename: ch_dns_rdata_rp.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 16:50:20
 * Last Modified: 2018-05-03 16:50:20
 */

#ifndef CH_DNS_RDATA_RP_H
#define CH_DNS_RDATA_RP_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_rp_t ch_dns_rdata_rp_t;


struct ch_dns_rdata_rp_t {

	ch_dns_rdata_t rdata;
	

	ch_dns_name_t mailbox;

	ch_dns_name_t textdomain;

};

extern void ch_dns_rdata_rp_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_RP_H*/
