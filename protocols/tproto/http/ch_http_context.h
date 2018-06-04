/*
 * =====================================================================================
 *
 *       Filename:  ch_http_context.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月12日 12时41分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_HTTP_CONTEXT_H
#define CH_HTTP_CONTEXT_H

typedef struct ch_http_context_t ch_http_context_t;

#include "ch_proto_context.h"

struct ch_http_context_t {

    ch_proto_context_t *pcontext;

	const char *fstore_dir;
	const char *req_body_dir;
	const char *res_body_dir;

	size_t fstore_buf_size;

	int create_body_dir_type;
	int create_dir_type;

	uint32_t n_entries_limit;
	uint32_t tbl_size;
	uint64_t entry_timeout;
	uint32_t n_caches_limits;

	size_t  parser_buf_size;

};

extern int ch_http_context_init(ch_proto_context_t *pcontext,ch_http_context_t *http_context);

extern ch_http_context_t *ch_http_context_create(ch_proto_context_t *pcontext);

#endif /*CH_HTTP_CONTEXT_H*/
