/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_proto.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 16:03:38
 * Last Modified: 2017-01-11 10:50:19
 */


#include <stdio.h>
#include <stdlib.h>

#include <apr_pools.h>

#include "ch_log.h"
#include "ch_proto_pool.h"
#include "ch_proto_context.h"
#include "ch_ftp_proto.h"
#include "ch_ftp_context.h"
#include "ch_ftp_parser_do.h"
#include "ch_session_format_factory.h"
#include "ch_ftp_session_format_msgpack.h"
#include "ch_ftp_session_tbl_ops.h"

ch_app_proto_t *
ch_ftp_proto_create(apr_pool_t *mp, ch_proto_context_t *pcontext)
{
	ch_ftp_proto_t *fpproto = NULL;
	ch_ftp_context_t *fcontext = NULL;
	ch_app_proto_t *aproto = NULL;
	apr_pool_t *htbl_mp = NULL;
	ch_file_store_t *fstore = NULL;

	fpproto = (ch_ftp_proto_t *) apr_palloc(mp, sizeof(*fpproto));

	fpproto->fcontext = ch_ftp_context_create(pcontext);

	if (fpproto->fcontext == NULL) {
		ch_log(CH_LOG_ERR, "create ftp context failed.");
		return NULL;
	}

	aproto = &fpproto->aproto;
	fcontext = fpproto->fcontext;

	aproto->trans_proto_type = TPT_TCP;
	aproto->proto_id = PROTOCOL_FTP;
	aproto->pcontext = pcontext;

	aproto->parse_request = ch_ftp_request_parse;
	aproto->parse_response = ch_ftp_response_parse;
	aproto->pcontext = NULL;

	aproto->priv_data = (void *) fcontext;

	fpproto->cmd_save_fpath = ch_fpath_create(mp, fcontext->cmd_save_dir,
		fcontext->create_cmd_dir_type, 0, pcontext->pid);
	if (fpproto->cmd_save_fpath == NULL) {
		ch_log(CH_LOG_ERR, "create fpath for ftp parser protocol failed.");
		return NULL;
	}

	fstore = ch_file_store_create(pcontext->mp, fcontext->fstore_dir,
		fcontext->fstore_buf_size, fcontext->create_dir_type, pcontext->pid);
	if (fstore == NULL) {
		ch_log(CH_LOG_ERR, "create file store for ftp proto failed.");
		ch_fpath_fin(fpproto->cmd_save_fpath);

		return NULL;
	}

	aproto->fmt = ch_session_format_create(pcontext, fstore, ch_ftp_session_format);
	if (aproto->fmt == NULL) {
		ch_log(CH_LOG_ERR, "create ftp session format failed.");
		ch_file_store_close(fstore);
		ch_fpath_fin(fpproto->cmd_save_fpath);

		return NULL;
	}

	apr_pool_create(&htbl_mp, NULL);
	if (htbl_mp == NULL) {
		ch_log(CH_LOG_ERR,"create ftp session table memory pool failed!");
		ch_file_store_close(fstore);
	    ch_fpath_fin(fpproto->cmd_save_fpath);

		return NULL;
	}

	aproto->htbl = ch_table_create(htbl_mp, 0,
		fcontext->n_entries_limit,
		fcontext->tbl_size,
		fcontext->entry_timeout,
		fcontext->n_caches_limits,
		(void *) aproto,
		ftp_session_entry_hash,
		ftp_session_entry_equal,
		ftp_session_entry_clean,
		ftp_session_entry_create);
	if (aproto->htbl == NULL) {
		ch_log(CH_LOG_ERR, "create ftp session table failed.");
		ch_file_store_close(fstore);
	    ch_fpath_fin(fpproto->cmd_save_fpath);

		return NULL;
	}

	return (ch_app_proto_t *) fpproto;
}

