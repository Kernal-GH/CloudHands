/*
 *
 *      Filename: do_mail_other.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-18 15:36:57
 * Last Modified: 2018-07-30 10:51:43
 */

static void *do_mail_session_entry_create(ch_tcp_app_t *app ,ch_proto_session_store_t *pstore){

	ch_pool_t *mp;

	ch_mail_session_entry_t *msentry = NULL;

    private_mail_context_t *mcontext = (private_mail_context_t*)app->context;

    ch_proto_body_store_t *bstore = ch_proto_store_body_find_create(pstore,
            mcontext->content_dir,
            mcontext->attach_dir,
            mcontext->create_body_dir_type,
            PROTOCOL_SMTP); 

    if(bstore == NULL){

		ch_log(CH_LOG_ERR,"Cannot create body store  for mail session entry!");
		return NULL;
    
    }

	mp = ch_pool_create(4096);

	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create memory pool for mail session entry!");
		return NULL;
	}

	msentry = (ch_mail_session_entry_t*)ch_pcalloc(mp,sizeof(*msentry));

	ch_mail_sentry_init(msentry,bstore,mp);


	return (void*)msentry;

}

#define MAIL_CAN_STORE(session) ((session)->mime_state!=NULL) 

static void do_mail_session_entry_clean(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession){

	ch_mail_session_entry_t *msentry = (ch_mail_session_entry_t*)tsession->sentry;
	
	ch_mail_session_t *session = NULL;;

	list_for_each_entry(session, &msentry->session_list, anchor) {
		if (!MAIL_CAN_STORE(session)) {
			continue;
		}

		ch_log(CH_LOG_DEBUG, "Mail session entry clean, subject: %.*s, from:%.*s.",
			session->subject.len, session->subject.data,
			session->mail_from.len, session->mail_from.data);

		session->username = &msentry->login;
		session->password = &msentry->passwd;

		ch_mail_session_flush(session);

        ch_proto_session_store_write(pstore,tsession,(void*)session); 

		ch_mail_session_destroy(session);
	}



	ch_mail_tx_buffer_clean(&msentry->cur_req_buf);
	ch_mail_tx_buffer_clean(&msentry->cur_res_buf);
	
	ch_pool_destroy(msentry->mp);
}

