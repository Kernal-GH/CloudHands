/*
 *
 *      Filename: ch_dns_proto.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-30 19:48:01
 * Last Modified: 2016-10-30 19:48:01
 */

#ifndef CH_DNS_PROTO_H
#define CH_DNS_PROTO_H

typedef struct ch_dns_proto_t ch_dns_proto_t;

#include "ch_dns_context.h"
#include "ch_proto_context.h"
#include "ch_proto_pool.h"
#include "ch_fpath.h"

struct ch_dns_proto_t {

	ch_app_proto_t aproto;

	ch_dns_context_t *dcontext;

	apr_pool_t *cur_mp;
	size_t sessions_parsed;
};


extern ch_app_proto_t * ch_dns_proto_create(ch_proto_context_t *pcontext);

extern apr_pool_t * ch_dns_proto_mp_get(ch_dns_proto_t *dns_proto);

#endif /* CH_DNS_PROTO_H */
