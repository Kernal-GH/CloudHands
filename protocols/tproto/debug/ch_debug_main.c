/*
 *
 *      Filename: ch_debug_main.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 23:11:01
 * Last Modified: 2016-12-06 00:48:32
 */

static inline int _debug_fpath_init(ch_debug_proto_t *dpproto,ch_debug_session_entry_t *dsentry,int is_req){

	FILE *fp = is_req?dsentry->req_fp:dsentry->res_fp;
	ch_fpath_t *fpath = is_req?dpproto->req_fpath:dpproto->res_fpath;

	char *fname,*fname_tmp;

	if(fp == NULL){
	
		ch_fpath_name_create(fpath,&fname,&fname_tmp);
		
		if(fname == NULL){
		
			ch_log(CH_LOG_ERR,"Can not create a file used to store debug body!");
			return -1;
		}

		return ch_debug_sentry_fpath_init(dsentry,(const char*)fname,is_req);

	}

	return 0;
} 

static int debug_request_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data)
{
	/* unused */
	priv_data = priv_data;
	
	ch_debug_proto_t *dpproto = (ch_debug_proto_t *) aproto;
	ch_debug_session_entry_t *dsentry = (ch_debug_session_entry_t *) sentry->priv_data;

	if(_debug_fpath_init(dpproto,dsentry,1))
		return PARSE_BREAK; 


	if(data&&dlen>0)
		ch_debug_sentry_data_write(dsentry,data,(size_t)dlen,1);

	return PARSE_CONTINUE;
}

int debug_response_parse(ch_app_proto_t *aproto,
	ch_session_entry_t *sentry, void *data,
	uint32_t dlen, void *priv_data)
{
	/* unused */
	priv_data = priv_data;

	ch_debug_proto_t *dpproto = (ch_debug_proto_t *) aproto;
	ch_debug_session_entry_t *dsentry = (ch_debug_session_entry_t *) sentry->priv_data;

	if(_debug_fpath_init(dpproto,dsentry,0))
		return PARSE_BREAK; 


	if(data&&dlen>0)
		ch_debug_sentry_data_write(dsentry,data,(size_t)dlen,0);

	return PARSE_CONTINUE;


}

