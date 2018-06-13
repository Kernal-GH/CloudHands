/*
 *
 *      Filename: ch_dns_rdata_key.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_KEY_H
#define CH_DNS_RDATA_KEY_H

typedef struct ch_dns_rdata_key_t ch_dns_rdata_key_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_sec_key.h"

struct ch_dns_rdata_key_t {

	ch_dns_rdata_t rdata;

	ch_dns_sec_key_t skey;
};


extern void ch_dns_rdata_key_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_KEY_H*/
