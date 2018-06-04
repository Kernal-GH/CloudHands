/*
 *
 *      Filename: ./protocols/mail/ch_mail_parser_do.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 17:36:16
 * Last Modified: 2016-10-28 02:03:30
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "smtp_parser.h"
#include "imap_parser.h"
#include "pop3_parser.h"
#include "ch_proto_pool.h"
#include "ch_table.h"
#include "ch_session_entry.h"
#include "ch_mail_proto.h"
#include "ch_mail_session_entry.h"

int mail_request_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	int ret = -1;
	ch_mail_proto_t *mpproto = (ch_mail_proto_t *) aproto;
	ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t *) sentry->priv_data;

	switch (aproto->proto_id) {
	case PROTOCOL_SMTP:
		ret = mail_smtp_request_parse(mpproto, msentry, data, dlen, priv_data);
		break;
	case PROTOCOL_POP3:
		ret = mail_pop3_request_parse(mpproto, msentry, data, dlen, priv_data);
		break;
	case PROTOCOL_IMAP:
		ret = mail_imap_request_parse(mpproto, msentry, data, dlen, priv_data);
		break;
	default:
		break;
	}

	return ret;
}

int mail_response_parse(ch_app_proto_t *aproto,
	ch_session_entry_t *sentry, void *data,
	uint32_t dlen, void *priv_data)
{
	int ret = -1;
	ch_mail_proto_t *mpproto = (ch_mail_proto_t *) aproto;
	ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t *) sentry->priv_data;

	switch (aproto->proto_id) {
	case PROTOCOL_SMTP:
		ret = mail_smtp_response_parse(mpproto, msentry, data, dlen, priv_data);
		break;
	case PROTOCOL_POP3:
		ret = mail_pop3_response_parse(mpproto, msentry, data, dlen, priv_data);
		break;
	case PROTOCOL_IMAP:
		ret = mail_imap_response_parse(mpproto, msentry, data, dlen, priv_data);
		break;
	default:
		break;
	}

	return ret;
}
