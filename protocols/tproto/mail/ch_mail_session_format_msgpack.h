/*
 *
 *      Filename: ./protocols/mail/ch_mail_session_format_msgpack.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 15:13:14
 * Last Modified: 2016-08-01 15:13:14
 */

#ifndef __CH_MAIL_SESSION_FORMAT_MSGPACK_H__
#define __CH_MAIL_SESSION_FORMAT_MSGPACK_H__

#include "ch_mail_session.h"
#include "ch_mail_session_entry.h"
int
mail_session_format(ch_session_format_t *fmt, void *session_in, void *priv_data);

void
mail_format_cur_session(ch_app_proto_t *aproto, 
	ch_mail_session_entry_t *msentry, ch_mail_session_t *session);

#endif //__CH_MAIL_SESSION_FORMAT_MSGPACK_H__

