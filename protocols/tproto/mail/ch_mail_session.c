/*
 *
 *      Filename: ./protocols/mail/ch_mail_session.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-02 11:44:27
 * Last Modified: 2016-12-13 11:20:10
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_mail_file.h"
#include "ch_mail_session.h"
#include "ch_mmap_file_format.h"

static inline void
ch_mail_session_init(ch_mail_session_t *session)
{
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
    CH_INIT_LIST_HEAD(&session->attach_list);

	session->mail_to_cnt = 0;
    CH_INIT_LIST_HEAD(&session->mail_to_list);

	session->mail_cc_cnt = 0;
    CH_INIT_LIST_HEAD(&session->mail_cc_list);
}

ch_mail_session_t *
ch_mail_session_create(apr_pool_t *mp)
{
	ch_mail_session_t *session;

	session = (ch_mail_session_t *) apr_palloc(mp, sizeof(*session));
	if (session == NULL) {
		CH_ERR_LOG("%s: apr_palloc failed.\n", __func__);
		return NULL;
	}

	ch_mail_session_init(session);

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
