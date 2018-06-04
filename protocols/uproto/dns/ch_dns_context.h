/*
 *
 *      Filename: ch_dns_context.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-30 19:19:05
 * Last Modified: 2016-10-30 19:19:05
 */

#ifndef CH_DNS_CONTEXT_H
#define CH_DNS_CONTEXT_H

typedef struct ch_dns_context_t ch_dns_context_t;

#include "ch_proto_context.h"

/* store some dns config informations*/

struct ch_dns_context_t {

	ch_proto_context_t *pcontext;
	
	const char *fstore_dir;
	
	size_t fstore_buf_size;

	int create_dir_type;

	size_t sessions_limit_mp;
};

extern ch_dns_context_t * ch_dns_context_create(ch_proto_context_t *pcontext);

#endif /* CH_DNS_CONTEXT_H */


