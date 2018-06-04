/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_proto.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 14:05:10
 * Last Modified: 2017-01-04 16:39:38
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_fpath.h"
#include "ch_proto_pool.h"
#include "ch_telnet_proto.h"
#include "ch_telnet_context.h"
#include "ch_telnet_parser_do.h"
#include "ch_session_format_factory.h"
#include "ch_telnet_session_format_msgpack.h"
#include "ch_telnet_session_tbl_ops.h"

ch_app_proto_t *
ch_telnet_proto_create(apr_pool_t *mp, ch_proto_context_t *pcontext)
{
	ch_telnet_proto_t *tpproto = NULL;
	ch_telnet_context_t *tcontext = NULL;
	ch_app_proto_t *aproto = NULL;

	apr_pool_t *htbl_mp = NULL;

	ch_file_store_t *fstore = NULL;

	tpproto = (ch_telnet_proto_t *) apr_palloc(mp, sizeof(*tpproto));

	tpproto->tcontext = ch_telnet_context_create(pcontext);
	if (tpproto->tcontext == NULL) {
		ch_log(CH_LOG_ERR, "create telnet context failed.");
		return NULL;
	}

	aproto = &tpproto->aproto;
	tcontext = tpproto->tcontext;

	aproto->trans_proto_type = TPT_TCP;
	aproto->proto_id = PROTOCOL_TELNET;  
	aproto->pcontext = pcontext;

	aproto->parse_request = ch_telnet_request_parse;
	aproto->parse_response = ch_telnet_response_parse;
	aproto->is_request = NULL;

	aproto->priv_data = (void*)tcontext;

	tpproto->trans_fpath = ch_fpath_create(mp, tcontext->trans_data_dir,
		tcontext->create_data_dir_type, 0, pcontext->pid);

	if (tpproto->trans_fpath == NULL) {
		ch_log(CH_LOG_ERR, "create fpath failed for telnet parser protocol.");
		return NULL;
	}

	fstore = ch_file_store_create(pcontext->mp, tcontext->fstore_dir,
		tcontext->fstore_buf_size, tcontext->create_dir_type, pcontext->pid);

	if (fstore == NULL) {
		ch_log(CH_LOG_ERR, "create file store failed for telnet proto!");
	    ch_fpath_fin(tpproto->trans_fpath);

		return NULL;
	}

	aproto->fmt = ch_session_format_create(pcontext, fstore, ch_telnet_session_format);
	if (aproto->fmt == NULL) {
		ch_log(CH_LOG_ERR, "create telnet session format failed.");
		ch_file_store_close(fstore);
	    ch_fpath_fin(tpproto->trans_fpath);

		return NULL;
	}

	apr_pool_create(&htbl_mp, NULL);
	if (htbl_mp == NULL) {
		ch_log(CH_LOG_ERR,"create telnet session table memory pool failed!");
		ch_file_store_close(fstore);
	    ch_fpath_fin(tpproto->trans_fpath);

		return NULL;
	}

	aproto->htbl = ch_table_create(htbl_mp, 0,
		tcontext->n_entries_limit,
		tcontext->tbl_size,
		tcontext->entry_timeout,
		tcontext->n_caches_limits,
		(void *) aproto,
		telnet_session_entry_hash,
		telnet_session_entry_equal,
		telnet_session_entry_clean,
		telnet_session_entry_create);

	if (aproto->htbl == NULL) {
		ch_log(CH_LOG_ERR, "create telnet session table failed.");
		ch_file_store_close(fstore);
	    ch_fpath_fin(tpproto->trans_fpath);

		return NULL;
	}

	return (ch_app_proto_t *) tpproto;
}

