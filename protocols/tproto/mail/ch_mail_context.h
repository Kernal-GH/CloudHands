/*
 *
 *      Filename: ./protocols/mail/ch_mail_context.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-29 17:11:13
 * Last Modified: 2016-07-29 17:11:13
 */

#ifndef __CH_PARSER_MAIL_CONTEXT_H__
#define __CH_PARSER_MAIL_CONTEXT_H__

#include <stdint.h>

#include "ch_proto_context.h"

typedef struct ch_mail_context_t ch_mail_context_t;

struct ch_mail_context_t {
    ch_proto_context_t *pcontext;

	const char *fstore_dir; /* store path for msgpack */
	const char *content_dir; /* store path for mail content */
	const char *attach_dir; /* store path for mail attachment */

	size_t fstore_buf_size; /* max buf size for magpack */

	int create_dir_type;	/* dir type for msgpack save */
	int create_body_dir_type; /* dir type for profile content and attach dir */

	uint32_t n_entries_limit;
	uint32_t tbl_size;
	uint64_t entry_timeout;
	uint32_t n_caches_limits;
};

ch_mail_context_t *ch_mail_context_create(ch_proto_context_t *pcontext);

#endif //__CH_PARSER_MAIL_CONTEXT_H__
