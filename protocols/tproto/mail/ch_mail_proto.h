/*
 *
 *      Filename: ./protocols/mail/ch_parser_mail_proto.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-29 16:35:17
 * Last Modified: 2016-07-29 16:35:17
 */

#ifndef __CH_PARSER_MAIL_PROTO_H__
#define __CH_PARSER_MAIL_PROTO_H__

#include "ch_fpath.h"
#include "apr_pools.h"
#include "ch_proto_pool.h"
#include "ch_mail_context.h"

typedef struct ch_mail_proto_s {
	ch_app_proto_t aproto; /* should be in the first of the struct */

	ch_mail_context_t *mcontext;

	ch_fpath_t *content_fpath;
	ch_fpath_t *attach_fpath;
} ch_mail_proto_t;

int ch_mail_proto_register(ch_proto_pool_t *ppool);

#endif //__CH_PARSER_MAIL_PROTO_H__
