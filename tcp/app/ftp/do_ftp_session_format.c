/*
 *
 *      Filename: do_ftp_session_format.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-25 13:49:39
 * Last Modified: 2018-09-25 15:19:22
 */

static void _ftp_ans_format(msgpack_packer *pk,ch_ftp_cmd_t *ftp_cmd){

	ch_ftp_ans_t *ftp_ans;

	list_for_each_entry(ftp_ans,&ftp_cmd->ans_list,node){
		ch_msgpack_map_start(pk,"ans",2);
		ch_msgpack_write_uint16(pk,"code",ftp_ans->code);
		ch_msgpack_write_kv(pk,"phrase",ftp_ans->phrase);
	}

}

static void _ftp_cmds_format(msgpack_packer *pk,ch_ftp_session_t *ftp_session) {

	ch_ftp_cmd_t *ftp_cmd;

	list_for_each_entry(ftp_cmd,&ftp_session->cmd_list,node){
	
		ch_msgpack_map_start(pk,"cmd",3);
		ch_msgpack_write_kv(pk,"cmdName",ftp_cmd->cmd);
		ch_msgpack_write_kv(pk,"cmdArgs",ftp_cmd->args);
		ch_msgpack_array_start(pk,"ansList",ftp_cmd->ans_n);
		if(ftp_cmd->ans_n){
		
			_ftp_ans_format(pk,ftp_cmd);

		}
	}
}

static void do_ftp_session_format(msgpack_packer *pk,void *session_in) {


	ch_ftp_session_entry_t *ftp_session_entry = (ch_ftp_session_entry_t*)session_in;
	ch_ftp_session_t *ftp_session = ftp_session_entry->ftp_session;

	ch_msgpack_map_start(pk,"ftpSession",3);
	ch_msgpack_write_kv(pk,"user",ftp_session->user);
	ch_msgpack_write_kv(pk,"passwd",ftp_session->passwd);
	ch_msgpack_array_start(pk,"cmdList",ftp_session->cmd_n);

	if(ftp_session->cmd_n){
	
		_ftp_cmds_format(pk,ftp_session);

	}
}



