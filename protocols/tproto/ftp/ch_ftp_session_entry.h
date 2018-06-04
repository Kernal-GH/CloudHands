/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_session_entry.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 15:13:59
 * Last Modified: 2017-01-04 15:13:59
 */


#ifndef __CH_FTP_SESSION_ENTRY_H__
#define __CH_FTP_SESSION_ENTRY_H__

#include <apr_pools.h>

#include "ch_ftp_proto.h"
#include "ch_ftp_session.h"
#include "ch_session_entry.h"
#include "ch_ftp_tx_buffer.h"
#include "ch_mmap_file_format.h"

typedef struct ch_ftp_session_entry_s {
	ch_session_entry_t *sentry;

	ch_ftp_proto_t *fpproto;

	apr_pool_t *mp;

	/* current line buffer */
	ch_ftp_tx_buf_t cur_req_buf;
	ch_ftp_tx_buf_t cur_res_buf;

	ch_ftp_session_t *session;
} ch_ftp_session_entry_t;

void ch_ftp_sentry_init(ch_ftp_session_entry_t *fsentry,ch_session_entry_t *sentry, 
	ch_mmap_file_entry_t *fentry, apr_pool_t *mp, ch_ftp_proto_t *fpproto);

#endif //__CH_FTP_SESSION_ENTRY_H__

