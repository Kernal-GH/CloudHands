/*
 *
 *      Filename: ch_http_parser.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 17:04:28
 * Last Modified: 2018-05-14 17:04:28
 */

#ifndef CH_HTTP_PARSER_H
#define CH_HTTP_PARSER_H

#include "ch_pp_pool.h"
#include "ch_buffer.h"
#include "ch_fpath.h"
#include "ch_http_context.h"

typedef struct ch_http_parse_context_t ch_http_parse_context_t;

struct ch_http_parse_context_t {

	ch_buffer_t g_buffer;

	ch_http_context_t *hcontext;

	ch_fpath_t *req_fpath;
	ch_fpath_t *res_fpath;

};

extern int ch_http_parser_init(ch_pp_pool_t *pp_pool,const char *cfname);

#endif /*CH_HTTP_PARSER_H*/
