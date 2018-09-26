/*
 *
 *      Filename: do_ftp_session_entry_create.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 11:53:13
 * Last Modified: 2018-09-26 19:59:19
 */

static void *do_ftp_session_entry_create(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore){

	ch_pool_t *mp;
	ch_ftp_session_entry_t *ftp_session_entry;
	ch_ftp_session_t *ftp_session;

	mp = ch_pool_create(1024);
	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create memory pool for ftp session entry!");
		return NULL;
	}

	ftp_session_entry = (ch_ftp_session_entry_t*)ch_pcalloc(mp,sizeof(*ftp_session_entry));
	ftp_session = ch_ftp_session_create(mp);

	if(ftp_session == NULL){
	
		ch_log(CH_LOG_ERR,"Create ftp session failed for ftp session entry!");
		ch_pool_destroy(mp);
		return NULL;
	}

	ch_ftp_session_entry_init(ftp_session_entry,mp,ftp_session,&pstore->g_buffer);

	return ftp_session_entry;
}

static void do_ftp_session_entry_clean(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession){

	
	ch_ftp_session_entry_t *entry = (ch_ftp_session_entry_t*)tsession->sentry;
	ch_ftp_session_t *ftp_session = entry->ftp_session;
	//ch_ftp_cmd_t *ftp_cmd;

	if(ftp_session&&ftp_session->cmd_n>0){
	
		ch_proto_session_store_write(pstore,tsession,(void*)entry);

#if 0
		if(ftp_session->cmd_n){
		
			list_for_each_entry(ftp_cmd,&ftp_session->cmd_list,node){
			
				if(ftp_cmd->ftp_dcon){
				
					ch_ftp_data_connection_fin(pstore->task_id,ftp_cmd->ftp_dcon);

				}
			}
		}
#endif

	}

	ch_pool_destroy(entry->mp);

}
