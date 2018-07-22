/*
 *
 *      Filename: do_mail_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-18 13:51:05
 * Last Modified: 2018-05-18 17:41:46
 */

static int do_mail_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        void *data,size_t dlen){

	int ret = PARSE_BREAK;
	switch (app->protocol_id) {
	case PROTOCOL_SMTP:
		ret = mail_smtp_request_parse(app,pstore,tsession, data, dlen);
		break;
	case PROTOCOL_POP3:
		ret = mail_pop3_request_parse(app,pstore,tsession, data, dlen);
		break;

	case PROTOCOL_IMAP:
		ret = mail_imap_request_parse(app,pstore,tsession, data, dlen);
		break;
	default:
		break;
	}

	return ret;

}

static int do_mail_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,
        void *data,size_t dlen){

	int ret = PARSE_BREAK;

	switch (app->protocol_id) {
	case PROTOCOL_SMTP:
		ret = mail_smtp_response_parse(app,pstore,tsession, data, dlen);
		break;
	case PROTOCOL_POP3:
		ret = mail_pop3_response_parse(app,pstore,tsession, data, dlen);
		break;
	case PROTOCOL_IMAP:
		ret = mail_imap_response_parse(app,pstore,tsession, data, dlen);
		break;
	default:
		break;
	}

	return ret;

}

