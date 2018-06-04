#ifndef __POP3_PARSER_H__
#define __POP3_PARSER_H__
/*
 *
 *      Filename: ./pop3_parser.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-01 16:24:50
 * Last Modified: 2016-07-01 16:24:50
 */

#include <stdbool.h>

#include "ch_list.h"
#include "ch_mail_common.h"
#include "ch_mail_decode_mime.h"
#include "ch_mail_session_entry.h"

typedef enum {
	psAuthorization = 0,
	psTransaction,
	psData,
	psUpdate
} Pop3_State;

typedef struct _pop3_state_s {
	int m_Is_Logined;

	int m_Cmd;

	int m_RetCode;

	Pop3_State  m_State;
} pop3_state_t;

int mail_pop3_request_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

int mail_pop3_response_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

#endif
