/*
 *
 *      Filename: ch_dns_rdata_tkey.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_TKEY_H
#define CH_DNS_RDATA_TKEY_H

typedef struct ch_dns_rdata_tkey_t ch_dns_rdata_tkey_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

struct ch_dns_rdata_tkey_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t alg;
	uint32_t time_inception;
	uint32_t time_expire;
	uint16_t mode;
	uint16_t error;

	uint16_t key_len;
	unsigned char *key;

	uint16_t other_len;
	unsigned char *other;

};


extern void ch_dns_rdata_tkey_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_TKEY_H*/
