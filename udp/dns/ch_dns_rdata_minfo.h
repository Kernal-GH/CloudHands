/*
 *
 *      Filename: ch_dns_rdata_minfo.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 15:58:47
 * Last Modified: 2018-05-03 15:58:47
 */

#ifndef CH_DNS_RDATA_MINFO_H
#define CH_DNS_RDATA_MINFO_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_minfo_t ch_dns_rdata_minfo_t;


struct ch_dns_rdata_minfo_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t responsibleAddress;

	ch_dns_name_t errorAddress;
};

extern void ch_dns_rdata_minfo_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_MINFO_H*/
