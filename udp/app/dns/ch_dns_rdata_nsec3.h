/*
 *
 *      Filename: ch_dns_rdata_nsec3.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_NSEC3_H
#define CH_DNS_RDATA_NSEC3_H

typedef struct ch_dns_rdata_nsec3_t ch_dns_rdata_nsec3_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

/*RFC5155*/

struct ch_dns_rdata_nsec3_t {

	ch_dns_rdata_t rdata;

	uint8_t hash_alg;
	uint8_t flags;
	uint16_t iterations;

	uint8_t salt_len;
	unsigned char *salt;

	uint8_t hash_len;
	unsigned char *next;

	uint16_t typebitmap_len;
	unsigned char *typebitmap;
};


extern void ch_dns_rdata_nsec3_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_NSEC3_H*/
