/*
 *
 *      Filename: ./cloudhands/protocols/mail/ch_mail_session_tbl_ops.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 10:58:34
 * Last Modified: 2016-12-14 19:00:04
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ch_log.h"
#include "ch_proto_pool.h"
#include "ch_mail_session.h"
#include "ch_session_entry.h"
#include "ch_table.h"
#include "ch_mmap_file_format.h"
#include "ch_mail_session_entry.h"

uint32_t 
mail_session_entry_hash(void *key, void *priv_data)
{

	/* unused */
	priv_data = priv_data;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;
	return fe->session_id;
}

int 
mail_session_entry_equal(ch_table_entry_t *entry, 
	void *key, void *priv_data)
{
	/* unused */
	priv_data = priv_data;


	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;
	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	return sentry->session_id == fe->session_id;
}

static inline int _mail_can_been_format(ch_mail_session_t *session){

	return (session->mime_state != NULL);
}

void 
mail_session_entry_clean(ch_table_entry_t *entry, void *priv_data)
{
	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t*)sentry->priv_data;
	
	ch_app_proto_t *aproto = (ch_app_proto_t*)priv_data;
	ch_mail_session_t *session = NULL;;

	list_for_each_entry(session, &msentry->session_list, anchor) {
		if (!_mail_can_been_format(session)) {
			continue;
		}

		ch_log(CH_LOG_DEBUG, "Mail session entry clean, subject: %.*s, from:%.*s.",
			session->subject.len, session->subject.data,
			session->mail_from.len, session->mail_from.data);

		session->username = &msentry->login;
		session->password = &msentry->passwd;

		ch_mail_session_flush(session);

		ch_session_format(aproto->fmt, sentry, (void*)session, sentry);

		ch_mail_session_destroy(session);
	}

	apr_pool_destroy(msentry->mp);

	ch_mail_tx_buffer_clean(&msentry->cur_req_buf);
	ch_mail_tx_buffer_clean(&msentry->cur_res_buf);
}

ch_table_entry_t * 
mail_session_entry_create(void *key, void *priv_data)
{
	apr_status_t rc;
	apr_pool_t *mp;
	ch_mail_session_entry_t *msentry = NULL;
	ch_session_entry_t *sentry = NULL;
	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	rc = apr_pool_create(&mp, NULL);

	if (rc != APR_SUCCESS || mp == NULL) {
		ch_log(CH_LOG_ERR, "Cannot create memory pool for http session entry!");
		return NULL;
	}

	sentry = (ch_session_entry_t *)apr_palloc(mp, sizeof(*sentry));
	msentry = (ch_mail_session_entry_t *)apr_palloc(mp, sizeof(*msentry));

	ch_mail_sentry_init(msentry, sentry,fe, mp, (ch_mail_proto_t *)priv_data);

	return (ch_table_entry_t *)sentry;
}


