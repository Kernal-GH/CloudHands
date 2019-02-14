/*
 *
 *      Filename: do_ftp_data_session_entry_create.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-20 16:52:47
 * Last Modified: 2018-09-26 19:28:34
 */


static void *do_ftp_data_session_entry_create(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused){

	const char *ftp_path;
	const char *ftp_fname;

	ch_pool_t *mp;
	
    ch_ftp_data_connection_t *ftp_dcon = (ch_ftp_data_connection_t*)app->context;
    ch_ftp_session_t *ftp_session = ftp_dcon->ftp_session;

	ch_ftp_data_session_entry_t *ftp_data_entry = NULL;

	mp = ftp_dcon->mp;


	ftp_data_entry = (ch_ftp_data_session_entry_t*)ch_pcalloc(mp,sizeof(*ftp_data_entry));

    if(ftp_data_entry == NULL){
    
        ch_log(CH_LOG_ERR,"Cannot create a ftp data session entry,because no memory!");
        return NULL;
    }


	ch_ftp_session_fpath_name_set(ftp_session,&ftp_path,&ftp_fname);

	ch_ftp_data_session_entry_init(ftp_data_entry,mp,ftp_dcon,ftp_path,ftp_fname);


	return (void*)ftp_data_entry;

}

#define FTP_DATA_SESSION_ENTRY_CAN_STORE(entry) ((entry!=NULL)&&((entry)->fstore_path!=NULL)&&(strlen((entry)->fstore_path)!=0))

static void do_ftp_data_session_entry_clean(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession) {

	ch_ftp_data_session_entry_t *entry = (ch_ftp_data_session_entry_t*)tsession->sentry;

    if(entry == NULL)
        return;

	if(FTP_DATA_SESSION_ENTRY_CAN_STORE(entry)){
		ch_proto_session_store_write(pstore,tsession,(void*)entry);	
	}

	ch_ftp_data_session_entry_fin(entry);

    entry->ftp_dcon->ftp_cmd->ftp_dcon = NULL;
    ch_ftp_data_connection_fin(pstore->task_id,entry->ftp_dcon);

}

