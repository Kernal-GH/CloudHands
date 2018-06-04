/*
 *
 *      Filename: ch_debug_tbl_ops.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 22:52:52
 * Last Modified: 2016-12-06 00:47:49
 */

static uint32_t debug_session_entry_hash(void *key,void *priv_data){

	/* unused  */
	priv_data = priv_data;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;
	return fe->session_id;
}

static int debug_session_entry_equal(ch_table_entry_t *entry,void *key,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	return sentry->session_id == fe->session_id;

}


static void debug_session_entry_clean(ch_table_entry_t *entry,void *priv_data){

	ch_app_proto_t *aproto = (ch_app_proto_t*)priv_data;
	
	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	ch_debug_session_entry_t *dsentry = (ch_debug_session_entry_t*)sentry->priv_data;

	/**/
	if (dsentry->req_fp||dsentry->res_fp) {
		ch_session_format(aproto->fmt,sentry,(void*)dsentry,NULL);
	
		ch_debug_session_entry_fin(dsentry);
	}
	
	apr_pool_destroy(dsentry->mp);

}

static ch_table_entry_t * debug_session_entry_create(void *key,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	apr_status_t rc;

	apr_pool_t *mp;
	ch_debug_session_entry_t *dsentry = NULL;
	ch_session_entry_t *sentry;

	ch_mmap_file_entry_t *fe = (ch_mmap_file_entry_t*)key;

	rc = apr_pool_create(&mp,NULL);

	if(rc!= APR_SUCCESS||mp == NULL){

		ch_log(CH_LOG_ERR,"Cannot create memory pool for debug session entry!");
		return NULL;
	}

	dsentry = (ch_debug_session_entry_t*)apr_palloc(mp,sizeof(ch_debug_session_entry_t));
	sentry = (ch_session_entry_t*)apr_palloc(mp,sizeof(ch_session_entry_t));

	ch_debug_sentry_init(dsentry,sentry,fe,mp);

	return (ch_table_entry_t*)sentry;
}

