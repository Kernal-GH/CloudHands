/*
 *
 *      Filename: ./protocols/mail/ch_mail_session_format_msgpack.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 11:34:18
 * Last Modified: 2016-10-28 02:04:52
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_mail_session.h"
#include "ch_session_format.h"
#include "ch_mail_common.h"
#include "ch_session_format_msgpack.h"
#include "ch_mail_session_entry.h"

static char *mail_get_proto_str(unsigned int proto_id)
{
	switch (proto_id) {
	case PROTOCOL_SMTP:
		return "SMTP";
	case PROTOCOL_POP3:
		return "POP3";
	case PROTOCOL_IMAP:
		return "IMAP";
	default:
		return NULL;
	}
}

static inline const char *_ctnt_path(ch_mail_ctnt_t *ctnt)
{
	if (ctnt) {
		return (const char *) ctnt->path;
	}

	return "";
}

static inline const char *_ctnt_name(ch_mail_ctnt_t *ctnt)
{
	if (ctnt) {
		return (const char *) ctnt->filename;
	}

	return "";
}

static inline void 
_pack_mail_attach(msgpack_packer *pk, ch_mail_session_t *s, const char *k, uint32_t n)
{
	ch_mail_ctnt_t *ctnt;

	ch_msgpack_array_start(pk, k, n);

	list_for_each_entry(ctnt, &s->attach_list, anchor) {
		ch_msgpack_map_start(pk, NULL, 2);
		ch_msgpack_write_kv(pk, "attach_name", _ctnt_name(ctnt));
		ch_msgpack_write_kv(pk, "attach_path", _ctnt_path(ctnt));
	}
}

static inline void 
_pack_mail_to(msgpack_packer *pk, ch_mail_session_t *s, const char *k, uint32_t n)
{
        char str[16] = {0};
	int i = 0;
	mail_addr_node_t *addr;

	ch_msgpack_map_start(pk, k, n);

	list_for_each_entry(addr, &s->mail_to_list, anchor) {
                snprintf(str, 16, "To%d", i++);
		ch_msgpack_write_kv(pk, str, (const char *) addr->name.data);
	}
}

static inline void 
_pack_mail_cc(msgpack_packer *pk, ch_mail_session_t *s, const char *k, uint32_t n)
{
        char str[16] = {0};
	int i = 0;
	mail_addr_node_t *addr;

	ch_msgpack_map_start(pk, k, n);

	list_for_each_entry(addr, &s->mail_cc_list, anchor) {
                snprintf(str, 16, "Cc%d", i++);
		ch_msgpack_write_kv(pk, str, (const char *) addr->name.data);
	}
}

/* called in parser function when a mail block is done. */
void
mail_format_cur_session(ch_app_proto_t *aproto, 
	ch_mail_session_entry_t *msentry, ch_mail_session_t *session)
{

	ch_mail_sentry_session_remove(msentry, session);

	session->username = &msentry->login;
	session->password = &msentry->passwd;

	ch_mail_session_flush(session);

	ch_session_format(aproto->fmt, msentry->sentry, (void *) session, msentry->sentry);

	ch_mail_session_destroy(session);
}

int
mail_session_format(ch_session_format_t *fmt, void *session_in, void *priv_data)
{
	ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;
	msgpack_packer *pk = &msgpack_fmt->pk;
	ch_mail_session_t *session = (ch_mail_session_t *)session_in;
	ch_session_entry_t *sentry = (ch_session_entry_t *) priv_data;

	if ( session == NULL) {
		CH_ERR_LOG("%s: session is NULL or mpproto is NULL.\n", __func__);
		return -1;
	}

	ch_msgpack_map_start(pk, "MAIL", 13);

	ch_msgpack_write_kv(pk, "protocol", mail_get_proto_str(sentry->protocol_id));

	ch_msgpack_write_kv(pk, "username", 
		session->username ? (const char *) session->username->data : "");
	ch_msgpack_write_kv(pk, "password", 
		session->password ? (const char *) session->password->data : "");

	ch_msgpack_write_kv(pk, "subject", (const char *) session->subject.data);
	ch_msgpack_write_kv(pk, "mail_from", (const char *) session->mail_from.data);

	ch_msgpack_write_uint(pk, "mail_to_cnt", session->mail_to_cnt);
	_pack_mail_to(pk, session, "mail_to", session->mail_to_cnt);

	ch_msgpack_write_uint(pk, "mail_cc_cnt", session->mail_cc_cnt);
	_pack_mail_cc(pk, session, "mail_cc", session->mail_cc_cnt);

	ch_msgpack_write_kv(pk, "content_text", _ctnt_path(session->ctnt_plain_file));

	ch_msgpack_write_kv(pk, "content_html", _ctnt_path(session->ctnt_html_file));

	ch_msgpack_write_uint(pk, "attach_cnt", session->attach_cnt);
	_pack_mail_attach(pk, session, "attachments", session->attach_cnt);

	
	return 0;
}

