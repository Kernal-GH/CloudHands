/*
 *
 *      Filename: ch_dns_rdata_cert.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_CERT_H
#define CH_DNS_RDATA_CERT_H

typedef struct ch_dns_rdata_cert_t ch_dns_rdata_cert_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

struct ch_dns_rdata_cert_t {

	ch_dns_rdata_t rdata;

	uint16_t cert_type;
	uint16_t key_tag;
	uint8_t alg;

	uint16_t cert_len;
	unsigned char *cert;

};


extern void ch_dns_rdata_cert_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_CERT_H*/
