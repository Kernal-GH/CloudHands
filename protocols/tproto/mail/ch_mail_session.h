/*
 *
 *      Filename: ./protocols/mail/ch_mail_session.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 16:40:30
 * Last Modified: 2016-08-01 16:40:30
 */

#ifndef __CH_MAIL_SESSION_H__
#define __CH_MAIL_SESSION_H__

#include "ch_list.h"
#include "ch_string.h"
#include "apr_pools.h"
#include "ch_mail_file.h"
#include "ch_mail_proto.h"
#include "ch_mail_decode_mime.h"

typedef struct ch_mail_session_s {
	struct ch_list_head anchor;

    int done;

    /** the first message contained in the session */
    MimeDecEntity *msg_head;
    /** the last message contained in the session */
    MimeDecEntity *msg_tail;
    /** the mime decoding parser state */
    MimeDecParseState *mime_state;

	ch_str_t *username;
	ch_str_t *password;

	ch_str_t mail_from;
	ch_str_t subject;

	ch_mail_ctnt_t *ctnt_plain_file;
	ch_mail_ctnt_t *ctnt_html_file;

	uint32_t attach_cnt;
	struct ch_list_head attach_list; 

	uint32_t mail_to_cnt;
	struct ch_list_head mail_to_list;  /* rcpt to string list */

	uint32_t mail_cc_cnt;
	struct ch_list_head mail_cc_list;

} ch_mail_session_t;

ch_mail_session_t *
ch_mail_session_create(apr_pool_t *mp);

void ch_mail_session_flush(ch_mail_session_t *session);

void ch_mail_session_destroy(ch_mail_session_t *session);

#endif 
