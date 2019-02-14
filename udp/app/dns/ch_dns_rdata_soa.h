/*
 *
 *      Filename: ch_dns_rdata_soa.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:08:57
 * Last Modified: 2018-05-03 19:08:57
 */

#ifndef CH_DNS_RDATA_SOA_H
#define CH_DNS_RDATA_SOA_H

#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

typedef struct ch_dns_rdata_soa_t ch_dns_rdata_soa_t;


struct ch_dns_rdata_soa_t {

	ch_dns_rdata_t rdata;

	ch_dns_name_t host; /*The primary name server for the zone*/
	
	ch_dns_name_t admin; /*The zone administrator's address*/

	uint32_t serial;  /*The zone's serial number*/
	uint32_t refresh; /*The amount of time until a secondary checks for a new serial number*/
	uint32_t retry; /*The amount of time between a secondary's checks for a new serial number*/
    uint32_t expire; /*The amount of time until a secondary expires a zone*/
	uint32_t minimum; /*The minimum TTL for records in the zone*/
};

extern void ch_dns_rdata_soa_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_SOA_H*/
