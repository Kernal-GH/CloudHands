/*
 *
 *      Filename: do_ftp_data_session_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 10:21:28
 * Last Modified: 2018-09-25 19:39:21
 */

#define FTP_DATA_NEED_CREATE_FILE_BODY(session) ((session)->fstore_fp == NULL)

static inline int _do_content_process(ch_tcp_app_t *app,ch_tcp_session_t *tsession,
        void *data,size_t dlen,int is_req){

	char *fname,*fname_tmp;
	ch_fpath_t *fpath = ch_ftp_fstore_path_get_from_app(app);

	ch_ftp_data_session_entry_t *ftp_data_entry = (ch_ftp_data_session_entry_t*)tsession->sentry;
	if(ftp_data_entry->ftp_dcon->tsession == NULL){
	
		ftp_data_entry->ftp_dcon->tsession = tsession;
	}

	ch_proto_session_entry_update(&ftp_data_entry->psEntry,dlen,is_req);


	if(FTP_DATA_NEED_CREATE_FILE_BODY(ftp_data_entry)){
	
		ch_fpath_name_create(fpath,&fname,&fname_tmp);
	
		if(fname == NULL){
		
			ch_log(CH_LOG_ERR,"Can not create a file used to store ftp_data body!");
			return -1;
		}

		if(ch_ftp_data_session_entry_fpath_init(ftp_data_entry,fname)){
		
			ch_log(CH_LOG_ERR,"Can not open a file used to store ftp_data body,fname:%s",fname);
			return -1;

		}
	}

	ch_ftp_data_session_entry_write(ftp_data_entry,data,dlen);

	return 0;
}


static int do_ftp_data_session_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused,
	ch_tcp_session_t *tsession,void *data,size_t dlen) {

	if(data!=NULL&&dlen>0){
	
		if(_do_content_process(app,tsession,data,dlen,1))
			return PARSE_BREAK;
	}

	return PARSE_CONTINUE;

} 

static int do_ftp_data_session_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused,
	ch_tcp_session_t *tsession,void *data,size_t dlen) {

	if(data!=NULL&&dlen>0){
	
		if(_do_content_process(app,tsession,data,dlen,0))
			return PARSE_BREAK;
	}

	return PARSE_CONTINUE;
}

