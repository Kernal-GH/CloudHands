/*
 *
 *      Filename: ./protocols/tproto/ftp/ch_ftp_session_tbl_ops.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-05 11:18:19
 * Last Modified: 2017-02-21 14:44:29
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ch_log.h"
#include "ch_ftp_session.h"
#include "ch_ftp_session_entry.h"
#include "ch_session_entry.h"
#include "ch_ftp_tx_buffer.h"
#include "ch_mmap_file_format.h"

uint32_t 
ftp_session_entry_hash(void *key, void *priv_data)
{
	/* unused */
	priv_data = priv_data;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	return fe->session_id;
}

int 
ftp_session_entry_equal(ch_table_entry_t *entry, 
	void *key, void *priv_data)
{
	/* unused */
	priv_data = priv_data;

	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;
	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	return sentry->session_id == fe->session_id;
}

static inline int _ftp_can_been_format(ch_ftp_session_t *session)
{
	return session->cmds_data != NULL;
}

void 
ftp_session_entry_clean(ch_table_entry_t *entry, void *priv_data)
{
	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	ch_ftp_session_entry_t *fsentry = (ch_ftp_session_entry_t*)sentry->priv_data;
	
	ch_app_proto_t *aproto = (ch_app_proto_t*)priv_data;
	ch_ftp_session_t *session = fsentry->session;;

	if (session == NULL || !_ftp_can_been_format(session)) {
		return;
	}

	ch_ftp_cmds_data_destroy(session->cmds_data);

	ch_session_format(aproto->fmt, sentry, (void*)session, sentry);

	ch_ftp_session_destroy(session);

	apr_pool_destroy(fsentry->mp);

	ch_ftp_tx_buffer_clean(&fsentry->cur_req_buf);
	ch_ftp_tx_buffer_clean(&fsentry->cur_res_buf);
}

ch_table_entry_t * 
ftp_session_entry_create(void *key, void *priv_data)
{
	apr_status_t rc;
	apr_pool_t *mp;
	ch_ftp_session_entry_t *fsentry = NULL;
	ch_session_entry_t *sentry = NULL;
	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	rc = apr_pool_create(&mp, NULL);

	if (rc != APR_SUCCESS || mp == NULL) {
		ch_log(CH_LOG_ERR, "Cannot create memory pool for http session entry!");
		return NULL;
	}

	sentry = (ch_session_entry_t *)apr_palloc(mp, sizeof(*sentry));
	fsentry = (ch_ftp_session_entry_t *)apr_palloc(mp, sizeof(*fsentry));

	ch_ftp_sentry_init(fsentry, sentry,fe, mp, (ch_ftp_proto_t *)priv_data);

	return (ch_table_entry_t *)sentry;
}


