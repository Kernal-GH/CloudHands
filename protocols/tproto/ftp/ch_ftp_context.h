/*
 *
 *      Filename: protocols/tproto/ftp/ch_ftp_context.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 15:40:55
 * Last Modified: 2017-01-04 15:40:55
 */

#ifndef __CH_FTP_CONTEXT_H__
#define __CH_FTP_CONTEXT_H__

#include "ch_proto_context.h"

typedef struct ch_ftp_context_s {
	ch_proto_context_t *pcontext;

	const char *fstore_dir; /* store path for msgpack */
	const char *cmd_save_dir; /* store path for ftp commands */

	size_t fstore_buf_size; /* max buf size for msgpack */

	int create_dir_type; /* dir type for msgpack save */
	int create_cmd_dir_type; /* dir type for transfer ftp commands */

	uint32_t n_entries_limit;
	uint32_t tbl_size;
	uint32_t entry_timeout;
	uint32_t n_caches_limits;
} ch_ftp_context_t;

ch_ftp_context_t *ch_ftp_context_create(ch_proto_context_t *pcontext);

#endif //__CH_FTP_CONTEXT_H__

