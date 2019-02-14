/*
 *
 *      Filename: ch_dns_rdata_x25.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 17:54:32
 * Last Modified: 2018-05-02 17:54:32
 */

#ifndef CH_DNS_RDATA_X25_H
#define CH_DNS_RDATA_X25_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"

typedef struct ch_dns_rdata_x25_t ch_dns_rdata_x25_t;


struct ch_dns_rdata_x25_t {

	ch_dns_rdata_t rdata;

	 unsigned char *address;

};

extern void ch_dns_rdata_x25_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_X25_H*/
