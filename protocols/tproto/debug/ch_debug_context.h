/*
 *
 *      Filename: ch_debug_context.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 21:57:13
 * Last Modified: 2016-12-05 21:57:13
 */

#ifndef CH_DEBUG_H
#define CH_DEBUG_H

typedef struct ch_debug_context_t ch_debug_context_t;

#include "ch_proto_context.h"

struct ch_debug_context_t {

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

};

extern ch_debug_context_t *ch_debug_context_create(ch_proto_context_t *pcontext);

#endif /* CH_DEBUG_H */
