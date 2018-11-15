/*
 *
 *      Filename: do_mail_format.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-18 12:00:59
 * Last Modified: 2018-05-18 18:08:04
 */

#define  _ctnt_path(ctnt) ((ctnt)&&(ctnt)->path?(const char*)(ctnt)->path:"")

#define  _ctnt_name(ctnt) ((ctnt)&&(ctnt)->filename?(const char*)(ctnt)->filename:"")

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

static void do_mail_session_format(msgpack_packer *pk, void *session_in)
{
	ch_mail_session_t *session = (ch_mail_session_t *)session_in;

	if ( session == NULL) {
		CH_ERR_LOG("%s: mail session is NULL.\n", __func__);
		return;
	}

	ch_msgpack_map_start(pk, "MAIL", 12);

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

	
}


