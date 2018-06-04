/*
 *
 *      Filename: ch_http_proto.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-19 23:42:55
 * Last Modified: 2016-07-19 23:42:55
 */

#ifndef CH_HTTP_PROTO_H
#define CH_HTTP_PROTO_H

typedef struct ch_http_proto_t ch_http_proto_t;

#include "ch_http_context.h"
#include "ch_proto_context.h"
#include "ch_proto_pool.h"
#include "ch_fpath.h"

struct ch_http_proto_t {

	ch_app_proto_t aproto;

	ch_http_context_t *hcontext;

	void *parser_buf;
	size_t parser_buf_size;
	ch_fpath_t *req_fpath;
	ch_fpath_t *res_fpath;
};


extern ch_app_proto_t * ch_http_proto_create(apr_pool_t *mp,ch_proto_context_t *pcontext);

extern int ch_http_proto_buf_init(ch_http_proto_t *hpproto,ch_buf_t *pb,ch_buf_t *remain_data,
	void *data,size_t dlen);

#endif /* CH_HTTP_PROTO_H*/
