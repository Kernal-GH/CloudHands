/*
 *
 *      Filename: do_smon_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-16 15:50:50
 * Last Modified: 2018-08-16 17:32:11
 */

#define SMON_NEED_CREATE_FILE_BODY(session,is_req) (is_req?(session)->req_content_fp == NULL:(session)->res_content_fp == NULL)

static inline int _do_content_process(ch_tcp_app_t *app,ch_proto_session_store_t *pstore, ch_tcp_session_t *tsession,
        void *data,size_t dlen,int is_req){

	char *fname,*fname_tmp;

    private_smon_context_t *mcontext = (private_smon_context_t*)app->context;
    
    ch_proto_body_store_t *bstore = ch_proto_store_body_find_create(pstore,mcontext->req_body_dir,mcontext->res_body_dir,
            mcontext->create_dir_type,app->protocol_id); 

    if(bstore == NULL){
    
        return -1;
    }

	ch_smon_session_entry_t *smon_entry = (ch_smon_session_entry_t*)tsession->sentry;

	ch_fpath_t *fpath = is_req?bstore->req_fpath:bstore->res_fpath;

	ch_proto_session_entry_update(&smon_entry->psEntry,dlen,is_req);


	if(SMON_NEED_CREATE_FILE_BODY(smon_entry,is_req)){
	
		ch_fpath_name_create(fpath,&fname,&fname_tmp);
	
		if(fname == NULL){
		
			ch_log(CH_LOG_ERR,"Can not create a file used to store smon body!");
			return -1;
		}

		if(ch_smon_session_entry_fpath_init(smon_entry,fname,is_req)){
		
			ch_log(CH_LOG_ERR,"Can not open a file used to store smon body,fname:%s",fname);
			return -1;

		}
	}

	ch_smon_session_entry_write(smon_entry,data,dlen,is_req);

	return 0;
}

static int 
do_smon_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
        ch_tcp_session_t *tsession,void *data,size_t dlen){

	if(_do_content_process(app,pstore,tsession,data,dlen,1))
		return PARSE_BREAK;

	return PARSE_CONTINUE;
}


static int 
do_smon_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
        ch_tcp_session_t *tsession,void *data,size_t dlen){

	if(_do_content_process(app,pstore,tsession,data,dlen,0))
		return PARSE_BREAK;

	return PARSE_CONTINUE;
}

