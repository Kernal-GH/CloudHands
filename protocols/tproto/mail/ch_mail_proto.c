/*
 *
 *      Filename: protocols/mail/ch_parser_mail_proto.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-29 16:33:07
 * Last Modified: 2016-11-05 21:28:03
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_proto_pool.h"
#include "ch_mail_context.h"
#include "ch_mail_parser_do.h"
#include "ch_mail_proto.h"
#include "ch_mail_session_tbl_ops.h"
#include "ch_session_format_factory.h"
#include "ch_mail_session_format_msgpack.h"

static ch_app_proto_t * 
_mail_proto_create(apr_pool_t *mp, 
					ch_proto_context_t *pcontext,
					ch_mail_context_t *mcontext,
					unsigned int proto_id)
{
	ch_mail_proto_t *mpproto = NULL;
	ch_app_proto_t *aproto = NULL;
	apr_pool_t *htbl_mp = NULL;
	ch_file_store_t *fstore = NULL;

	mpproto = (ch_mail_proto_t *) apr_palloc(mp, sizeof (*mpproto));

	mpproto->mcontext = mcontext;

	aproto = &mpproto->aproto;

	aproto->trans_proto_type = TPT_TCP;
	aproto->proto_id = proto_id;
	aproto->pcontext = pcontext;

	aproto->parse_request = mail_request_parse;
	aproto->parse_response = mail_response_parse;
	aproto->is_request = NULL;

	aproto->priv_data = (void *)mcontext;

	mpproto->content_fpath = ch_fpath_create(mp, mcontext->content_dir, 
		mcontext->create_body_dir_type, 0, pcontext->pid);

	mpproto->attach_fpath = ch_fpath_create(mp, mcontext->attach_dir, 
		mcontext->create_body_dir_type, 0, pcontext->pid);

	if (mpproto->content_fpath == NULL || \
		mpproto->attach_fpath == NULL)
	{
		ch_log(CH_LOG_ERR,"create fpath failed for mail parser protocol!");
		return NULL;
	}

	fstore = ch_file_store_create(pcontext->mp, mcontext->fstore_dir, 
		mcontext->fstore_buf_size, mcontext->create_dir_type, pcontext->pid);

	if (fstore == NULL) {
		ch_log(CH_LOG_ERR, "create file store failed for mail proto!");
		goto fail;
	}

	aproto->fmt = ch_session_format_create(pcontext, fstore, mail_session_format);
	if (aproto->fmt == NULL) {
		ch_log(CH_LOG_ERR, "create mail session format failed!");
		goto fail;
	}

	apr_pool_create(&htbl_mp, NULL);
	if (htbl_mp == NULL) {
		ch_log(CH_LOG_ERR, "create mail session table memory pool failed!");
		goto fail;
	}

	aproto->htbl = ch_table_create(htbl_mp,0,
		mcontext->n_entries_limit,
		mcontext->tbl_size,
		mcontext->entry_timeout,
		mcontext->n_caches_limits,
		(void*)aproto,
		mail_session_entry_hash,
		mail_session_entry_equal,
		mail_session_entry_clean,
		mail_session_entry_create);

	if (aproto->htbl == NULL) {
		ch_log(CH_LOG_ERR,"create http session table failed!");
		goto fail;
	}

	return aproto;

fail:
	ch_file_store_close(fstore);
	ch_fpath_fin(mpproto->content_fpath);
	ch_fpath_fin(mpproto->attach_fpath);

	return NULL;
}

int ch_mail_proto_register(ch_proto_pool_t *ppool)
{
	ch_app_proto_t *aproto = NULL;
	ch_mail_context_t *mcontext = NULL;
	ch_proto_context_t *pcontext = ppool->pcontext;
	apr_pool_t *mp = pcontext->mp;

	int proto_num = 0;

	mcontext = ch_mail_context_create(pcontext);
	if (mcontext == NULL) {
		ch_log(CH_LOG_ERR, "create mail context failed.\n");
		return -1;
	}

	aproto = _mail_proto_create(mp, pcontext, mcontext, PROTOCOL_POP3);
	if (aproto) {
		ch_app_proto_register(ppool,aproto);
		proto_num++;
	}

	aproto = _mail_proto_create(mp, pcontext, mcontext, PROTOCOL_IMAP);
	if (aproto) {
		ch_app_proto_register(ppool,aproto);
		proto_num++;
	}

	aproto = _mail_proto_create(mp, pcontext, mcontext, PROTOCOL_SMTP);
	if (aproto) {
		ch_app_proto_register(ppool,aproto);
		proto_num++;
	}

	return proto_num;
}
