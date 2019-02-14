/*
 *
 *      Filename: ./cloudhands/protocols/mail/ch_mail_session_entry.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 11:13:19
 * Last Modified: 2016-08-01 11:13:19
 */

#ifndef __CH_MAIL_SESSION_ENTRY_H__
#define __CH_MAIL_SESSION_ENTRY_H__

typedef struct ch_mail_session_entry_t ch_mail_session_entry_t;

#include "ch_mail_session.h"
#include "ch_mail_session_buffer.h"
#include "ch_list.h"
#include "ch_tcp_app_pool.h"

struct ch_mail_session_entry_t {
	
	ch_pool_t *mp;
    
    ch_proto_body_store_t *bstore;

	struct list_head session_list;
	ch_mail_session_t *curr_session;

	uint8_t parser_state;

	uint8_t current_command;

	/* current line buffer */
	ch_mail_tx_buf_t cur_req_buf;
	ch_mail_tx_buf_t cur_res_buf;

	/* login & passwd */
	ch_str_t login;
	ch_str_t passwd;


	/* internel use for SMTP/IMAP/POP3 */
	void *priv_data;

};


#define ch_mail_sentry_session_remove(msentry,session) do      \
{															   \
	list_del(&session->anchor);								   \
	if (session == msentry->curr_session) {			           \
		msentry->curr_session = NULL;						   \
	}														   \
}while(0)

#define ch_mail_parse_context_get(msentry) (ch_mail_parse_context_t*)((msentry)->sentry->pp_parser->parse_context)

extern void ch_mail_sentry_init(ch_mail_session_entry_t *msentry,ch_proto_body_store_t *bstore,ch_pool_t *mp);


#endif //__CH_MAIL_SESSION_ENTRY_H__

