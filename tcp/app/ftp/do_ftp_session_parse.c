/*
 *
 *      Filename: do_ftp_session_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 12:12:44
 * Last Modified: 2018-09-21 16:34:21
 */

static inline void _line_str_parse(ch_pp_data_line_t *line,ch_str_t *first,ch_str_t *second) {

	unsigned char *start = line->line;
	unsigned char *end = line->line+line->len;
	unsigned char *p;
	size_t i = 0;
	first->len = 0;
	first->data = NULL;

	second->len = 0;
	second->data = NULL;

	/*skip space*/
	for(p = start;p<end;p++){
	
		if(!isspace(*p))
			break;
	}

	if(p>=end)
		return;

	first->data = p;
	
	for(;p<end;p++){

		if(!isspace(*p)){
			first->len++;
		}else{
		
			break;
		}
	}
	
	for(;p<end;p++){

		if(!isspace(*p))
			break;
	}

	if(p<end){
	
		second->data = p;
		second->len = end-p;
	}
	
} 

static void _process_port_cmd(ch_tcp_app_t *app,ch_tcp_session_t *tsession,ch_ftp_session_entry_t *ftp_session_entry,
	ch_str_t *cmd,ch_str_t *cmd_args){


}

static void _process_pasv_ans(ch_tcp_app_t *app,ch_tcp_session_t *tsession,ch_ftp_session_entry_t *ftp_session_entry,
	ch_str_t *ans,ch_str_t *ans_args){


}

static int do_ftp_session_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
	ch_tcp_session_t *tsession,void *data,size_t dlen) {

	int rc;

	ch_str_t cmd_tmp,*cmd = &cmd_tmp;
	ch_str_t cmd_args_tmp,*cmd_args = &cmd_args_tmp;

	ch_ftp_session_entry_t *ftp_session_entry = (ch_ftp_session_entry_t*)tsession->sentry;
	ch_proto_session_entry_update(&ftp_session_entry->psEntry,dlen,1);

	ch_pp_data_input_t *din;
	ch_pp_data_line_t tmp,*line=&tmp;

	din = &ftp_session_entry->data_input;

	if(ch_pp_din_read_prefare(din,data,dlen)){
	
		ch_log(CH_LOG_ERR,"Cannot init data input to parse ftp resquest!");
		return PARSE_BREAK;
	}

	/*Try to read a line from data input*/
	if(ch_pp_data_input_line_read(din,line)==0){
		
		/*ok!*/
		ch_pp_din_skip_to_end(din);

		/*process cmd line*/
		_line_str_parse(line,cmd,cmd_args);

		if(cmd->len>0&&cmd->data!=NULL){
		
			ch_ftp_session_cmd_add(ftp_session_entry->ftp_session,cmd,cmd_args);

			if(IS_CMD_STR(cmd,CMD_PORT)){

				_process_port_cmd(app,tsession,ftp_session_entry,cmd,cmd_args);
			}
		}
	}

	if(ch_pp_din_read_end(din)){
	
		ch_log(CH_LOG_ERR,"Cannot complete data input end for parse ftp request!");
		return PARSE_BREAK;
	}

	return PARSE_CONTINUE;
}

static int do_ftp_session_response_parse() {

}

