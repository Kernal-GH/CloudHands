/*
 *
 *      Filename: ./cloudhands/protocols/mail/ch_mail_session_entry.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 11:13:19
 * Last Modified: 2016-08-01 11:13:19
 */

#ifndef __CH_MAIL_SESSION_ENTRY_H__
#define __CH_MAIL_SESSION_ENTRY_H__

#include "apr_pools.h"
#include "ch_mail_session.h"
#include "ch_mail_session_buffer.h"

typedef struct ch_mail_session_entry_s {
	
	ch_session_entry_t *sentry;

	ch_mail_proto_t *mpproto;

	apr_pool_t *mp;

	struct ch_list_head session_list;
	ch_mail_session_t *curr_session;

	uint8_t parser_state;

	uint8_t current_command;

	/* current line buffer */
	ch_mail_tx_buf_t cur_req_buf;
	ch_mail_tx_buf_t cur_res_buf;

	/* login & passwd */
	ch_str_t login;
	ch_str_t passwd;

	/* internel use for SMTP/IMAP/POP3 */
	void *priv_data;

} ch_mail_session_entry_t;

void ch_mail_sentry_init(ch_mail_session_entry_t *msentry,ch_session_entry_t *sentry, 
	ch_mmap_file_entry_t *fentry, apr_pool_t *mp, ch_mail_proto_t *mpproto);

static inline void 
ch_mail_sentry_session_remove(ch_mail_session_entry_t *msentry,
	ch_mail_session_t *session)
{
	list_del(&session->anchor);

	if (session == msentry->curr_session) {
		msentry->curr_session = NULL;
	}
}

#endif //__CH_MAIL_SESSION_ENTRY_H__


