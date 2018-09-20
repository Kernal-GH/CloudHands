/*
 *
 *      Filename: do_ftp_data_session_entry_create.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-20 16:52:47
 * Last Modified: 2018-09-20 18:45:27
 */


static void *do_ftp_data_session_entry_create(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused){

	const char *ftp_path;
	const char *ftp_fname;

	ch_pool_t *mp;
	ch_ftp_session_t *ftp_session = ch_ftp_session_get_from_app(app);

	ch_ftp_data_session_entry_t *ftp_data_entry = NULL;

	mp = ch_pool_create(512);

	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create memory pool for ftp_data session entry!");
		return NULL;
	}

	ftp_data_entry = (ch_ftp_data_session_entry_t*)ch_pcalloc(mp,sizeof(*ftp_data_entry));

	ch_ftp_session_fpath_name_set(ftp_session,&ftp_path,&ftp_fname);

	ch_ftp_data_session_entry_init(ftp_data_entry,mp,ftp_path,ftp_fname);


	return (void*)ftp_data_entry;

}

