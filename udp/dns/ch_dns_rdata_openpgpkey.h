/*
 *
 *      Filename: ch_dns_rdata_openpgpkey.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_OPENPGPKEY_H
#define CH_DNS_RDATA_OPENPGPKEY_H

typedef struct ch_dns_rdata_openpgpkey_t ch_dns_rdata_openpgpkey_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

struct ch_dns_rdata_openpgpkey_t {

	ch_dns_rdata_t rdata;

	uint16_t key_len;
	unsigned char *key;

};


extern void ch_dns_rdata_openpgpkey_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_OPENPGPKEY_H*/
