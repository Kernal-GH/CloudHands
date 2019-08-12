/*
 *
 *      Filename: ch_dns_request.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-07 15:20:14
 * Last Modified: 2018-05-07 15:20:14
 */

#ifndef CH_DNS_REQUEST_H
#define CH_DNS_REQUEST_H

typedef struct ch_dns_requst_t ch_dns_requst_t;
typedef struct ch_dns_question_t ch_dns_question_t;

#include "ch_dns_header.h"
#include "ch_dns_name.h"
#include "ch_mpool.h"
#include "ch_dns_data_input.h"
#include "ch_list.h"
#include "ch_data_output.h"
#include "ch_msgpack_store.h"

struct ch_dns_requst_t {

	ch_dns_header_t hdr;

	struct list_head questions;

};

struct ch_dns_question_t {

	struct list_head node;
	ch_dns_name_t qname;
	uint16_t qtype;
	uint16_t qclass;

};


extern ch_dns_requst_t *ch_dns_request_parse(ch_pool_t *mp,ch_dns_data_input_t *din);

extern ch_dns_question_t *ch_dns_question_parse(ch_pool_t *mp,ch_dns_data_input_t *din);

extern void ch_dns_request_dump(ch_dns_requst_t *dnsr,FILE *fp);

extern void ch_dns_question_dump(ch_dns_question_t *dnsq,FILE *fp);

extern const char * ch_dns_request_name_get(ch_dns_requst_t *dns_req);

extern void  ch_dns_request_store(ch_msgpack_store_t *dstore,ch_dns_requst_t *dnsr);
extern void  ch_dns_question_store(ch_dns_question_t *dnsq,ch_msgpack_store_t *dstore);

extern ssize_t ch_dns_question_write(ch_dns_question_t *dnsq,ch_data_output_t *dout);
#define CH_DNS_QUESTION_WRITE(dout,dnsq,len,rc) do {   \
	if(-1 == (rc = ch_dns_question_write(dnsq,dout)))  \
		return -1;									  \
	len+=rc;										  \
}while(0)


extern ssize_t ch_dns_request_write(ch_dns_requst_t *dnsr,ch_data_output_t *dout);

#define CH_DNS_REQUEST_WRITE(dout,dnsr,len,rc) do {   \
	if(-1 == (rc = ch_dns_request_write(dnsr,dout)))  \
		return -1;									  \
	len+=rc;										  \
}while(0)


#endif /*CH_DNS_REQUEST_H*/
