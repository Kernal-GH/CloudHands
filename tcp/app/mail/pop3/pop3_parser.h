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

extern int mail_pop3_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        void *data, size_t dlen);

extern int mail_pop3_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        void *data, size_t dlen);

#endif
