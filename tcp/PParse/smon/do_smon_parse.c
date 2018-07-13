/*
 *
 *      Filename: do_smon_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-16 15:50:50
 * Last Modified: 2018-07-13 15:02:16
 */

#define SMON_NEED_CREATE_FILE_BODY(session,is_req) (is_req?(session)->req_content_fp == NULL:(session)->res_content_fp == NULL)

static inline int _do_content_process(ch_session_entry_t *sentry,void *data,size_t dlen,int is_req){

	char *fname,*fname_tmp;

	ch_smon_parse_context_t *smon_parse_context = (ch_smon_parse_context_t*)sentry->pp_parser->parse_context;

	ch_smon_session_entry_t *smon_entry = ch_my_session_entry_get(sentry,ch_smon_session_entry_t);

	ch_fpath_t *fpath = is_req?smon_parse_context->req_fpath:smon_parse_context->res_fpath;


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
do_smon_request_parse(ch_session_entry_t *sentry,void *data,size_t dlen){

	if(_do_content_process(sentry,data,dlen,1))
		return PARSE_BREAK;

	return PARSE_CONTINUE;
}


static int 
do_smon_response_parse(ch_session_entry_t *sentry,void *data,size_t dlen){

	if(_do_content_process(sentry,data,dlen,0))
		return PARSE_BREAK;

	return PARSE_CONTINUE;
}

