/*
 *
 *      Filename: do_ftp_session_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 12:12:44
 * Last Modified: 2018-09-26 19:29:47
 */

static inline void _line_str_parse(ch_pp_data_line_t *line,ch_str_t *first,ch_str_t *second) {

	unsigned char *start = line->line;
	unsigned char *end = line->line+line->len;
	unsigned char *p;
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

static void _ip_port_get(const char *str,uint32_t *ip,uint16_t *port,int is_pasv) {

    int i = 0;
    uint16_t intv;
    uint32_t ip_v = 0;
    uint16_t port_v1 = 0;
    uint16_t port_v2 = 0;
    *ip = 0;
    *port = 0;

    char str_cp[64] = {0};

    size_t str_len = strlen(str);

    if(is_pasv){

        /*skip no digit char*/
        while(!isdigit(*str)&&(*str)!='\0'){
            str++;
            str_len--;
        };
        
        /*skip tail no digit char*/
        size_t nlen = strlen(str);
        const char *nstr = str+nlen-1;
        while(nstr>str&&!isdigit(*nstr)){
        
            nstr--;
            str_len--;
        }

    }

    memcpy(str_cp,str,str_len<=63?str_len:63);

    char *temp = strtok(str_cp,",");

    while(temp){
    
        if(i>=6)
            break;

        intv = atoi(temp);

        if(i<4){

            ip_v += intv<<(8*i);
        }else if(i==4){
        
            port_v1 = intv;
        }else if(i == 5){
        
            port_v2 = intv;
        }

        temp = strtok(NULL,",");
        i++;
    }

    *ip = ip_v;
    *port = (port_v1<<8)+port_v2;

}


static int do_ftp_session_request_parse(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused,
	ch_tcp_session_t *tsession,void *data,size_t dlen) {

	ch_str_t cmd_tmp,*cmd = &cmd_tmp;
	ch_str_t cmd_args_tmp,*cmd_args = &cmd_args_tmp;

	ch_ftp_session_entry_t *ftp_session_entry = (ch_ftp_session_entry_t*)tsession->sentry;
	ch_proto_session_entry_update(&ftp_session_entry->psEntry,dlen,1);

	ch_pp_data_input_t *din = &ftp_session_entry->data_input_req;
	ch_pp_data_line_t tmp,*line=&tmp;


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

		}
	}

	if(ch_pp_din_read_end(din)){
	
		ch_log(CH_LOG_ERR,"Cannot complete data input end for parse ftp request!");
		return PARSE_BREAK;
	}

	return PARSE_CONTINUE;
}

#define STRV_IS_EMPTY(v) ((v)==NULL||strlen(v)==0)

static void _process_ftp_data_connection(ch_proto_session_store_t *pstore,
	ch_tcp_session_t *tsession,ch_ftp_session_entry_t *ftp_session_entry,ch_ftp_cmd_t *ftp_cmd,
	 ch_ftp_ans_t *ftp_ans,int is_pasv){


	uint32_t t_dip;
	uint32_t t_sip;

	uint32_t dst_ip;
	uint16_t dst_port;
	uint32_t src_ip;

	ch_ftp_data_connection_t *ftp_dcon;
	
	t_dip = ch_tcp_session_dstip_get(tsession);
	t_sip = ch_tcp_session_srcip_get(tsession);

	if(STRV_IS_EMPTY(ftp_ans->phrase))
		return;

	if(ftp_cmd->ftp_dcon){
	
		ch_log(CH_LOG_ERR,"This FTP Cmd:%s,has been create a ftp data connection!",ftp_cmd->cmd);
		return;
	}

	if(is_pasv&&ftp_ans->code!=FTP_PASVOK){

		ch_log(CH_LOG_ERR,"ignore this failed pasv cmd process:%lu,%s",(unsigned long)ftp_ans->code,ftp_ans->phrase);
		return;
	}

	if(is_pasv == 0&&ftp_ans->code!=FTP_PORTOK){
	
		ch_log(CH_LOG_ERR,"ignore this failed port cmd process:%lu,%s",(unsigned long)ftp_ans->code,ftp_ans->phrase);
		return;
	}

	_ip_port_get(ftp_ans->phrase,&dst_ip,&dst_port,is_pasv);
	if((dst_ip == 0||(dst_ip!=t_sip&&dst_ip!=t_dip))||dst_port == 0){
	
		ch_log(CH_LOG_ERR,"ignore this ftp cmd:%s,invalid ip&port:%s",ftp_cmd->cmd,ftp_ans->phrase);
		return;
	}
	
	src_ip = dst_ip == t_sip?t_dip:t_sip;

	/*create a ftp data connection*/
	ftp_dcon = ch_ftp_data_connection_create(pstore->task_id,ftp_session_entry->ftp_session,
		src_ip,dst_ip,dst_port);

	if(ftp_dcon == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create a ftp data connection for ftp_cmd:%s %s,ftp_ans:%d %s",
			ftp_cmd->cmd,ftp_cmd->args,(int)ftp_ans->code,ftp_ans->phrase);

	}
    ftp_dcon->ftp_cmd = ftp_cmd;
	ftp_cmd->ftp_dcon = ftp_dcon;

}

static void _process_ftp_data_rw_ok(ch_proto_session_store_t *pstore,
	ch_tcp_session_t *tsession,ch_ftp_session_entry_t *ftp_session_entry){


	ch_ftp_cmd_t *ftp_cmd;

	ftp_cmd = ch_ftp_session_find_last_cmd(ftp_session_entry->ftp_session,CMD_PASV);
	if(ftp_cmd == NULL){
	
		ftp_cmd = ch_ftp_session_find_last_cmd(ftp_session_entry->ftp_session,CMD_PORT);
	}
	
	if(ftp_cmd == NULL){
	
		ch_log(CH_LOG_ERR,"No pasv/port ftp cmd has been found!");
		return;
	}

	if(ftp_cmd->ftp_dcon){
	
		/**/
		ch_ftp_data_connection_fin_output(pstore->task_id,pstore,tsession,ftp_cmd->ftp_dcon);
		/*reset*/
		ftp_cmd->ftp_dcon = NULL;

	}

}

static int _process_ftp_ans(ch_proto_session_store_t *pstore,
        ch_tcp_session_t *tsession,ch_ftp_session_entry_t *ftp_session_entry,ch_ftp_cmd_t *ftp_cmd,
        ch_ftp_ans_t *ftp_ans) {

	int rc = PARSE_CONTINUE;

	if(IS_CMD(ftp_cmd,CMD_PASV)){
		
		_process_ftp_data_connection(pstore,tsession,ftp_session_entry,ftp_cmd,ftp_ans,1);

	}else if(IS_CMD(ftp_cmd,CMD_PORT)){

		_process_ftp_data_connection(pstore,tsession,ftp_session_entry,ftp_cmd,ftp_ans,0);
	}else if(ftp_ans->code == FTP_TRANSFEROK){
		
		_process_ftp_data_rw_ok(pstore,tsession,ftp_session_entry);

	}

	return rc;
}

static int 
do_ftp_session_response_parse(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore,
        ch_tcp_session_t *tsession,void *data,size_t dlen) {


	int rc = PARSE_CONTINUE;

    ch_ftp_ans_t *ftp_ans = NULL;

	ch_str_t code_tmp,*code = &code_tmp;
	ch_str_t phrase_tmp,*phrase = &phrase_tmp;

	ch_ftp_session_entry_t *ftp_session_entry = (ch_ftp_session_entry_t*)tsession->sentry;
	ch_proto_session_entry_update(&ftp_session_entry->psEntry,dlen,0);

	ch_pp_data_input_t *din = &ftp_session_entry->data_input_res;
	ch_pp_data_line_t tmp,*line=&tmp;


	if(ch_pp_din_read_prefare(din,data,dlen)){
	
		ch_log(CH_LOG_ERR,"Cannot init data input to parse ftp response!");
		return PARSE_BREAK;
	}

    while(ch_pp_din_has_data(din,0)){

        /*Try to read a line from data input*/
        if(ch_pp_data_input_line_read(din,line)==0){
            
            /*process cmd line*/
            _line_str_parse(line,code,phrase);

            if(code->len>0&&code->data!=NULL){

                ftp_ans = ch_ftp_session_ans_add(ftp_session_entry->ftp_session,code,phrase);
                if(ftp_ans&&ftp_ans->cmd){
                
                   rc =  _process_ftp_ans(pstore,tsession,ftp_session_entry,ftp_ans->cmd,ftp_ans);
                }
            }
        }else{
            /*cannot read a line ,maybe need more data*/
            break;
        }

    }


	if(ch_pp_din_read_end(din)){
	
		ch_log(CH_LOG_ERR,"Cannot complete data input end for parse ftp request!");
		return PARSE_BREAK;
	}

	return rc;

}

