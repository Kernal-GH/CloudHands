/*
 *
 *      Filename: ch_http_session_tbl_ops.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-20 00:23:00
 * Last Modified: 2016-11-22 17:44:55
 */


static uint32_t http_session_entry_hash(void *key,void *priv_data){

	/* unused  */
	priv_data = priv_data;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;
	return fe->session_id;
}

static int http_session_entry_equal(ch_table_entry_t *entry,void *key,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	return sentry->session_id == fe->session_id;

}

static inline int _can_been_format(ch_http_session_t *session){

	return session->parse_phase >= PARSE_PHASE_REQ_HEADER;
}

static void http_session_entry_clean(ch_table_entry_t *entry,void *priv_data){

	ch_http_session_t *session = NULL;
	ch_app_proto_t *aproto = (ch_app_proto_t*)priv_data;
	
	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	ch_http_session_entry_t *hsentry = (ch_http_session_entry_t*)sentry->priv_data;

	/**/
	list_for_each_entry(session,&hsentry->reqs,node){
	
		if (_can_been_format(session)) {
			ch_session_format(aproto->fmt,sentry,(void*)session,NULL);
		}

		ch_http_session_body_destroy(session->req_body);
		ch_http_session_body_destroy(session->res_body);
	}


	apr_pool_destroy(hsentry->mp);

}

static ch_table_entry_t * http_session_entry_create(void *key,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	apr_status_t rc;

	apr_pool_t *mp;
	ch_http_session_entry_t *hsentry = NULL;
	ch_session_entry_t *sentry;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	rc = apr_pool_create(&mp,NULL);

	if(rc!= APR_SUCCESS||mp == NULL){

		ch_log(CH_LOG_ERR,"Cannot create memory pool for http session entry!");
		return NULL;
	}

	hsentry = (ch_http_session_entry_t*)apr_palloc(mp,sizeof(ch_http_session_entry_t));
	sentry = (ch_session_entry_t*)apr_palloc(mp,sizeof(ch_session_entry_t));

	ch_http_sentry_init(hsentry,sentry,fe,mp);

	return (ch_table_entry_t*)sentry;
}


