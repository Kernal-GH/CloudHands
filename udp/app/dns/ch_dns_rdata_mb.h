/*
 *
 *      Filename: ch_dns_rdata_mb.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:04:07
 * Last Modified: 2018-05-03 10:04:07
 */

#ifndef CH_DNS_RDATA_MB_H
#define CH_DNS_RDATA_MB_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_mb_t ch_dns_rdata_mb_t;


struct ch_dns_rdata_mb_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t name;

};

extern void ch_dns_rdata_mb_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_MB_H*/
