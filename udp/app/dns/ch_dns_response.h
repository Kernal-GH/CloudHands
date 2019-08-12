/*
 *
 *      Filename: ch_dns_response.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-07 17:41:56
 * Last Modified: 2018-05-07 17:41:56
 */

#ifndef CH_DNS_RESPONSE_H
#define CH_DNS_RESPONSE_H

#include "ch_dns_header.h"
#include "ch_dns_request.h"
#include "ch_dns_data_input.h"
#include "ch_mpool.h"
#include "ch_dns_name.h"
#include "ch_dns_rdata.h"
#include "ch_dns_rdata_pool.h"
#include "ch_data_output.h"

typedef struct ch_dns_response_t ch_dns_response_t;

struct ch_dns_response_t {

	ch_dns_header_t hdr;

	struct list_head qlist;
	struct list_head alist;
	struct list_head aulist;
	struct list_head adlist;

};

extern ch_dns_response_t* ch_dns_response_parse(ch_pool_t *mp,ch_dns_rdata_pool_t *rdata_pool,ch_dns_data_input_t *din);

extern void ch_dns_response_dump(ch_dns_response_t *dnsr,FILE *fp);

extern ssize_t ch_dns_response_write(ch_dns_response_t *dnsr,ch_data_output_t *dout);

extern void ch_dns_response_store(ch_msgpack_store_t *dstore,ch_dns_response_t *dnsr);

#define CH_DNS_RESPONSE_WRITE(dout,dnsr,len,rc) do {   \
	if(-1 == (rc = ch_dns_response_write(dnsr,dout)))  \
		return -1;									   \
	len+=rc;										   \
}while(0)

#endif /*CH_DNS_RESPONSE_H*/
