/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_context.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 14:26:50
 * Last Modified: 2016-12-09 14:26:50
 */

#ifndef __CH_TELNET_CONTEXT_H__
#define __CH_TELNET_CONTEXT_H__

#include "ch_proto_context.h"

typedef struct ch_telnet_context_s {
	ch_proto_context_t *pcontext;

	const char *fstore_dir; /* store path for msgpack */
	const char *trans_data_dir; /* store path for transfer data */

	size_t fstore_buf_size; /* max buf size for magpack */

	int create_dir_type; /* dir type for msgpack save */
	int create_data_dir_type; /* dir type for transfer data */

	uint32_t n_entries_limit;
	uint32_t tbl_size;
	uint64_t entry_timeout;
	uint32_t n_caches_limits;
} ch_telnet_context_t;

ch_telnet_context_t *ch_telnet_context_create(ch_proto_context_t *pcontext);

#endif //__CH_TELNET_CONTEXT_H__


