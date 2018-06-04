/*
 *
 *      Filename: do_mail_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-18 13:51:05
 * Last Modified: 2018-05-18 17:41:46
 */

static int do_mail_request_parse(ch_session_entry_t *sentry,void *data,size_t dlen){

	int ret = -1;
	ch_mail_session_entry_t *msentry  = ch_my_session_entry_get(sentry,ch_mail_session_entry_t);

	switch (sentry->protocol_id) {
	case PROTOCOL_SMTP:
		ret = mail_smtp_request_parse(msentry, data, dlen);
		break;
	case PROTOCOL_POP3:
		ret = mail_pop3_request_parse(msentry, data, dlen);
		break;
	case PROTOCOL_IMAP:
		ret = mail_imap_request_parse(msentry, data, dlen);
		break;
	default:
		break;
	}

	return ret;

}

static int do_mail_response_parse(ch_session_entry_t *sentry,void *data,size_t dlen){

	int ret = -1;
	ch_mail_session_entry_t *msentry  = ch_my_session_entry_get(sentry,ch_mail_session_entry_t);

	switch (sentry->protocol_id) {
	case PROTOCOL_SMTP:
		ret = mail_smtp_response_parse(msentry, data, dlen);
		break;
	case PROTOCOL_POP3:
		ret = mail_pop3_response_parse(msentry, data, dlen);
		break;
	case PROTOCOL_IMAP:
		ret = mail_imap_response_parse(msentry, data, dlen);
		break;
	default:
		break;
	}

	return ret;

}

