/*
 *
 *      Filename: ch_dns_rdata_nsec3param.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_NSEC3PARAM_H
#define CH_DNS_RDATA_NSEC3PARAM_H

typedef struct ch_dns_rdata_nsec3param_t ch_dns_rdata_nsec3param_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

struct ch_dns_rdata_nsec3param_t {

	ch_dns_rdata_t rdata;

	uint8_t hash_alg;
	uint8_t flags;
	uint16_t iterations;

	uint8_t salt_len;
	unsigned char *salt;

};


extern void ch_dns_rdata_nsec3param_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_NSEC3PARAM_H*/
