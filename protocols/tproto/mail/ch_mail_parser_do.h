/*
 *
 *      Filename: ./protocols/mail/ch_mail_parser_do.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 17:39:53
 * Last Modified: 2016-08-01 17:39:53
 */

#ifndef __CH_MAIL_PARSER_DO_H__
#define __CH_MAIL_PARSER_DO_H__


int mail_request_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *tentry, void *data, 
	uint32_t dlen, void *priv_data);

int mail_response_parse(ch_app_proto_t *aproto,
	ch_session_entry_t *tentry, void *data,
	uint32_t dlen, void *priv_data);

#endif //__CH_MAIL_PARSER_DO_H__

