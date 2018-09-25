/*
 *
 *      Filename: ch_ftp_session.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 16:34:34
 * Last Modified: 2018-09-25 16:46:28
 */

#include "ch_ftp_session.h"
#include "ch_log.h"

ch_ftp_session_t * ch_ftp_session_create(ch_pool_t *mp) {


	ch_ftp_session_t *ftp_session = ch_pcalloc(mp,sizeof(*ftp_session));

	if(ftp_session == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create ftp session,no memory!");
		return NULL;
	}

	INIT_LIST_HEAD(&ftp_session->cmd_list);
	
	ftp_session->cur_cmd = NULL;
	ftp_session->mp = mp;
	ftp_session->user = NULL;
	ftp_session->passwd = NULL;
	ftp_session->cmd_n = 0;

	return ftp_session;
}


ch_ftp_cmd_t* ch_ftp_session_cmd_add(ch_ftp_session_t *ftp_session,ch_str_t *cmd_str,ch_str_t *args_str) {

	ch_ftp_cmd_t *ftp_cmd;

	const char *cmd = NULL;
	const char *cmd_args = NULL;

	if(cmd_str->data == NULL||cmd_str->len == 0)
		return NULL;

	cmd = (const char*)ch_pstrndup(ftp_session->mp,(const char*)cmd_str->data,cmd_str->len);

	if(args_str->data&&args_str->len>0){
		cmd_args = (const char*)ch_pstrndup(ftp_session->mp,(const char*)args_str->data,args_str->len);
	}

	if(ftp_session->user == NULL&&IS_CMD_STR(cmd_str,CMD_USER)){
		
		ftp_session->user = cmd_args;

	}else if(ftp_session->passwd == NULL &&IS_CMD_STR(cmd_str,CMD_PASS)){
	
		ftp_session->passwd = cmd_args;
	}

	/*create a cmd*/
	ftp_cmd = ch_pcalloc(ftp_session->mp,sizeof(*ftp_cmd));

	ftp_cmd->cmd = cmd;
	ftp_cmd->args = cmd_args;
	ftp_cmd->ftp_dcon = NULL;
	ftp_cmd->ans_n = 0;
	ftp_session->cmd_n++;

	INIT_LIST_HEAD(&ftp_cmd->ans_list);

	list_add(&ftp_cmd->node,&ftp_session->cmd_list);
	ftp_session->cur_cmd = ftp_cmd;

	return ftp_cmd;
}

static inline uint16_t str_to_uint16(ch_str_t *code_str) {

	unsigned char *start = code_str->data;
	unsigned char *end = code_str->data+code_str->len;
	unsigned char *p;
	uint16_t code = 0;

	for(p = start;p<end;p++){
	
		if(!isdigit(*p))
			break;

		code =  code*10+(*p-48);
	}

	return code;
}


ch_ftp_ans_t* ch_ftp_session_ans_add(ch_ftp_session_t *ftp_session,ch_str_t *code_str,ch_str_t *phrase_str){

	ch_ftp_cmd_t *ftp_cmd = ftp_session->cur_cmd;
	ch_ftp_ans_t *ftp_ans;

	uint16_t code = 0;
	const char *phrase = NULL;
	
	/*ignore no cmd's response*/
	if(ftp_cmd == NULL)
		return NULL;


	if(code_str->data == NULL||code_str->len == 0)
		return NULL;


	code = str_to_uint16(code_str); 
	if(code == 0){
	
		ch_log(CH_LOG_ERR,"Invalid ftp cmd response,code is 0!");
		return NULL;
	}

	if(phrase_str->data&&phrase_str->len>0){
		phrase = (const char*)ch_pstrndup(ftp_session->mp,(const char*)phrase_str->data,phrase_str->len);
	}

	/*create a ftp cmd ans*/
	ftp_ans = (ch_ftp_ans_t*)ch_pcalloc(ftp_session->mp,sizeof(*ftp_ans));
	ftp_ans->cmd = ftp_cmd;
	ftp_ans->code = code;
	ftp_ans->phrase = phrase;
	ftp_cmd->ans_n++;

	list_add_tail(&ftp_ans->node,&ftp_cmd->ans_list);

	return ftp_ans;
}


