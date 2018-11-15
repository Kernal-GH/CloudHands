/*
 *
 *      Filename: ch_dns_rdata_nsec.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_NSEC_H
#define CH_DNS_RDATA_NSEC_H

typedef struct ch_dns_rdata_nsec_t ch_dns_rdata_nsec_t;

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

/*RFC4034*/

struct ch_dns_rdata_nsec_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t name;

	uint16_t typebitmap_len;
	unsigned char *typebitmap;
};


extern void ch_dns_rdata_nsec_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_NSEC_H*/
