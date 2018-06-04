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
#include <stdint.h>

#include "ch_list.h"
#include "ch_string.h"
#include "ch_mail_proto.h"
#include "ch_mail_common.h"
#include "ch_mail_decode_mime.h"
#include "ch_mail_session_entry.h"

typedef struct imap_state_s {
	uint32_t body_len;
	uint32_t body_read;
} imap_state_t;

int mail_imap_request_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

int mail_imap_response_parse(ch_mail_proto_t *mpproto, 
	ch_mail_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

#endif //__CH_PARSER_IMAP_PARSER_H__
