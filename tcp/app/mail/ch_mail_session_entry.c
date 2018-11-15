/*
 *
 *      Filename: ./protocols/mail/ch_mail_session_entry.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-08-01 17:03:47
 * Last Modified: 2018-05-18 17:31:10
 */

#include "ch_mail_session_entry.h"

void ch_mail_sentry_init(ch_mail_session_entry_t *msentry,ch_proto_body_store_t *bstore,ch_pool_t *mp)
{
	msentry->mp = mp;
    msentry->bstore = bstore;
	
	INIT_LIST_HEAD(&msentry->session_list);

	msentry->curr_session = NULL;
	msentry->parser_state = 0;
	msentry->current_command = 0;

	ch_mail_tx_buffer_init(&msentry->cur_req_buf);
	ch_mail_tx_buffer_init(&msentry->cur_res_buf);

	msentry->login.data = NULL;
	msentry->login.len = 0;

	msentry->passwd.data = NULL;
	msentry->passwd.len = 0;

	msentry->priv_data = NULL;

}

