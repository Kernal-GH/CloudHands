/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session_tbl_ops.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 18:07:55
 * Last Modified: 2016-12-16 09:36:10
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_session_entry.h"
#include "ch_telnet_session.h"
#include "ch_mmap_file_format.h"
#include "ch_telnet_session_entry.h"

uint32_t 
telnet_session_entry_hash(void *key, void *priv_data)
{
	/* unused */
	priv_data = priv_data;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	return fe->session_id;
}

int 
telnet_session_entry_equal(ch_table_entry_t *entry, 
	void *key, void *priv_data)
{
	/* unused */
	priv_data = priv_data;

	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;
	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	return sentry->session_id == fe->session_id;
}

static inline int _telnet_can_been_format(ch_telnet_session_t *session)
{
	return session->trans_data != NULL;
}

void 
telnet_session_entry_clean(ch_table_entry_t *entry, void *priv_data)
{
	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	ch_telnet_session_entry_t *tsentry = (ch_telnet_session_entry_t*)sentry->priv_data;
	
	ch_app_proto_t *aproto = (ch_app_proto_t*)priv_data;
	ch_telnet_session_t *session = tsentry->cur_session;;

	if (!_telnet_can_been_format(session)) {
		return;
	}

	ch_telnet_transfer_data_destroy(session->trans_data);

	ch_session_format(aproto->fmt, sentry, (void*)session, sentry);

	ch_telnet_session_destroy(session);

	apr_pool_destroy(tsentry->mp);
}

ch_table_entry_t * 
telnet_session_entry_create(void *key, void *priv_data)
{
	apr_status_t rc;
	apr_pool_t *mp;
	ch_telnet_session_entry_t *tsentry = NULL;
	ch_session_entry_t *sentry = NULL;
	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	rc = apr_pool_create(&mp, NULL);

	if (rc != APR_SUCCESS || mp == NULL) {
		ch_log(CH_LOG_ERR, "Cannot create memory pool for http session entry!");
		return NULL;
	}

	sentry = (ch_session_entry_t *)apr_palloc(mp, sizeof(*sentry));
	tsentry = (ch_telnet_session_entry_t *)apr_palloc(mp, sizeof(*tsentry));

	ch_telnet_sentry_init(tsentry, sentry,fe, mp, (ch_telnet_proto_t *)priv_data);

	return (ch_table_entry_t *)sentry;
}


