/*
 *
 *      Filename: ch_dns_rdata_ipseckey.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:23:33
 * Last Modified: 2018-06-12 16:23:33
 */

#ifndef CH_DNS_RDATA_IPSECKEY_H
#define CH_DNS_RDATA_IPSECKEY_H

typedef struct ch_dns_rdata_ipseckey_t ch_dns_rdata_ipseckey_t;

#include <sys/socket.h>     
#include <netinet/in.h>     
#include <arpa/inet.h> 
#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_name.h"

struct ch_dns_rdata_ipseckey_t {

	ch_dns_rdata_t rdata;

	uint8_t precedence;
	uint8_t gateway_type;
	uint8_t alg;
	
	struct in_addr      in_addr;    /* gateway type 1 */                         
	struct in6_addr     in6_addr;   /* gateway type 2 */                         
	ch_dns_name_t       gateway;    /* gateway type 3 */                             

	uint16_t        key_len;                              
	unsigned char       *key;                                                    
};


extern void ch_dns_rdata_ipseckey_init(ch_dns_rdata_pool_t *rdata_pool);

#endif /*CH_DNS_RDATA_IPSECKEY_H*/
