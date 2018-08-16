/*
 *
 *      Filename: ./protocols/mail/ch_mail_session.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-02 11:44:27
 * Last Modified: 2018-08-16 17:35:17
 */

#include "ch_mail_session.h"
#include "ch_log.h"
#include "ch_mail_file.h"

static inline void _mail_session_init(ch_mail_session_t *session)
{
	ch_proto_session_entry_init(&session->psEntry);

	session->done = 0;

	session->msg_head = NULL;
	session->msg_tail = NULL;
	session->mime_state = NULL;

	session->username = NULL;
	session->password = NULL;

	session->mail_from.data = NULL;
	session->mail_from.len = 0;

	session->subject.data = NULL;
	session->subject.len = 0;

	session->ctnt_plain_file = NULL;
	session->ctnt_html_file = NULL;

	session->attach_cnt = 0;
    INIT_LIST_HEAD(&session->attach_list);

	session->mail_to_cnt = 0;
    INIT_LIST_HEAD(&session->mail_to_list);

	session->mail_cc_cnt = 0;
    INIT_LIST_HEAD(&session->mail_cc_list);
}

ch_mail_session_t *ch_mail_session_create(ch_pool_t *mp)
{
	ch_mail_session_t *session;

	session = (ch_mail_session_t *) ch_palloc(mp, sizeof(*session));
	if (session == NULL) {
		CH_ERR_LOG("%s: palloc failed.\n", __func__);
		return NULL;
	}

	_mail_session_init(session);

	return session;
}

void ch_mail_session_flush(ch_mail_session_t *session)
{
	ch_mail_ctnt_t *ctnt = NULL;
	if (!session)
		return;

	ctnt = session->ctnt_plain_file;
	if (ctnt && ctnt->saved != 1) {
		ch_mail_ctnt_close(ctnt);
	}

	ctnt = session->ctnt_html_file;
	if (ctnt && ctnt->saved != 1) {
		ch_mail_ctnt_close(ctnt);
	}

	list_for_each_entry(ctnt, &session->attach_list, anchor) {
		if (ctnt && ctnt->saved != 1) {
			ch_mail_ctnt_close(ctnt);
		}
	}
}

void ch_mail_session_destroy(ch_mail_session_t *session)
{
	/* Notes: only the memory for mime decode should be freed */
	if (session->msg_head)
		MimeDecFreeEntity(session->msg_head);

	/* De Init parser */
	if (session->mime_state)
		MimeDecDeInitParser(session->mime_state);
}

/* called in parser function when a mail block is done. */
void
ch_mail_session_store(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        ch_mail_session_entry_t *msentry, ch_mail_session_t *session)
{

	ch_mail_sentry_session_remove(msentry, session);

	session->username = &msentry->login;
	session->password = &msentry->passwd;

	ch_mail_session_flush(session);
	
	if(ch_proto_session_store_write(pstore,tsession,(void*)session)){
	
		ch_log(CH_LOG_ERR,"Write mail session failed!");

	}

	ch_mail_session_destroy(session);
}

