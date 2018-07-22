#ifndef __CH_PARSER_IMAP_PARSER_H__
#define __CH_PARSER_IMAP_PARSER_H__
/*
 *
 *      Filename: ./imap/imap_parser.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-06 15:59:07
 * Last Modified: 2016-07-06 15:59:07
 */

#include "ch_mail_session_entry.h"

typedef struct imap_state_s {
	uint32_t body_len;
	uint32_t body_read;
} imap_state_t;

extern int mail_imap_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        void *data,size_t dlen);

extern int mail_imap_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession, 
        void *data,size_t dlen);

#endif //__CH_PARSER_IMAP_PARSER_H__
