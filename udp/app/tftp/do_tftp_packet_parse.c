/*
 *
 *      Filename: do_tftp_packet_parse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-14 13:36:08
 * Last Modified: 2018-09-14 15:00:18
 */

#define IS_NEED_CREATE_FSTORE(session) ((session)->fp == NULL)

static inline int _do_file_content_process(ch_tftp_session_t *tftp_session,private_tftp_app_context_t *context,
	void *data,size_t dlen) {

	char *fname,*fname_tmp;

	ch_fpath_t *fpath = context->fstore_fpath;

	if(IS_NEED_CREATE_FSTORE(tftp_session)){
	
		ch_fpath_name_create(fpath,&fname,&fname_tmp);
	
		if(fname == NULL){
		
			ch_log(CH_LOG_ERR,"Can not create a file used to store tftp file content!");
			return -1;
		}

		if(ch_tftp_session_fpath_init(tftp_session,fname)){
		
			ch_log(CH_LOG_ERR,"Can not open a file used to store tftp file content,fname:%s",fname);
			return -1;

		}
	}

	ch_tftp_session_file_write(tftp_session,data,dlen);
	return 0;
}

static int do_tftp_packet_parse(ch_tftp_session_t *tftp_session,private_tftp_app_context_t *context,void *data,size_t dlen) {

	int rc;
	uint16_t opcode;
	uint16_t block;

	void *p = data;

	if(data == NULL ||dlen<4){
		ch_log(CH_LOG_ERR,"Invalid tftp packet,discard it!");
		return PROCESS_CONTINUE;
	}

	/*read opcode*/
	opcode = READ_UINT16(p);
	p=READ_SKIP(p,2);

	/*only process data/ack/error packet!*/
	if(opcode<=OP_WRQ||opcode>OP_ERROR){
		ch_log(CH_LOG_ERR,"This tftp packet should not process here,discard it!");
		return PROCESS_CONTINUE;
	}

	switch(opcode){

	case OP_DATA:
		
		rc =  PROCESS_CONTINUE;
		block = READ_UINT16(p);
		p=READ_SKIP(p,2);
		
		if(block-tftp_session->last_block>1){
			/**/
			ch_log(CH_LOG_ERR,"This tftp data packet is over than last block to 1 block,so maybe some packet has not been captured!");
			rc = PROCESS_ERR;
		}else{
			
			tftp_session->last_block = block;

			if(_do_file_content_process(tftp_session,context,p,dlen-4)){
			
				/*write error*/
				ch_log(CH_LOG_ERR,"Write data info file failed!");
				rc = PROCESS_ERR;
			}else{
			
				if(dlen-4<DATA_SIZE){
				
					/*This is last block*/
					rc = PROCESS_DONE;
				}
			}
		}
		break;

	case OP_ACK:
		
		rc = PROCESS_CONTINUE;
		block = READ_UINT16(p);
		if(block){
			if(block!=tftp_session->last_block){
				ch_log(CH_LOG_ERR,"This tftp packet ack is over last read/write data block,so maybe some packet has not been captured!");
				rc = PROCESS_ERR;
			}
		}
		break;

	case OP_ERROR:

		p=READ_SKIP(p,2);
		tftp_session->is_error =1;
		tftp_session->errmsg = (const char*)ch_pstrdup(tftp_session->mp,(const char*)p);

		rc = PROCESS_DONE;
		break;

	default:
		break;
	}

	return rc;
}

