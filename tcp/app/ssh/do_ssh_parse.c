/*
 * =====================================================================================
 *
 *       Filename:  do_ssh_parse.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/14/2018 03:52:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */


static int _parse_ssh_version(ch_ssh_session_entry_t *ssh_entry,ch_ssh_data_t *ssh_data,ch_pp_data_input_t *din){

	ch_pp_data_line_t tmp,*line=&tmp;

    size_t dlen = ch_pp_din_rdlen(din);

    if(dlen<6){
        /*need more data*/
        return 1;
    }

    if(strncasecmp((const char*)din->pos, "SSH-", 4)){
        /* is a invalid ssh packet*/
        return -1;
    }
        /*Try to read a line from data input*/
    if(ch_pp_data_input_line_read(din,line)==0){

        if(line->len>=6){

            if(!strncasecmp((const char*)line->line, "SSH-1.", 6)){

                if((line->len>7)&&(line->line[6]=='9')&&(line->line[7]=='9')){
                    /*  SSH 1.99 which is the same as SSH2.0 */
                    ssh_data->version = SSH_VERSION_2;
                }else{
                    ssh_data->version = SSH_VERSION_1;
                }
            }else if(!strncasecmp((const char*)line->line, "SSH-2.", 6)){

                ssh_data->version = SSH_VERSION_2;
            }

        }else{

            ssh_data->version = SSH_VERSION_UNKNOWN;
        }
        
        ssh_data->proto_version = (const char*)ch_pstrndup(ssh_entry->mp,(const char*)line->line,line->len);

        ssh_data->state = SSH_STATE_VERSION;

    }else {

        /*need more data*/
        return 1;
    } 

    return 0;
}

static int _ssh_packet_parse(ch_ssh_packet_t *ssh_packet,ch_pp_data_input_t *din){


    uint32_t pkt_len=0;
    size_t dlen = ch_pp_din_rdlen(din);

    if(dlen<6){
        return -1; /*errror*/
    }

    pkt_len = ch_pp_din_uint32_read(din);
    if(pkt_len<2){

        return -1; /*error*/
    }

    ssh_packet->pkt_len = pkt_len;
    ssh_packet->pad_len = ch_pp_din_uint8_read(din);
    ssh_packet->msg_code = ch_pp_din_uint8_read(din);

    /* ok! */
    return 0;
}

static int _ssh_parse_record(ch_ssh_session_entry_t *ssh_entry ch_unused,ch_ssh_data_t *ssh_data,ch_pp_data_input_t *din)
{

    ch_ssh_packet_t ssh_pkt_tmp,*ssh_pkt = &ssh_pkt_tmp;

    size_t input_len = 0;

    while(ch_pp_din_has_data(din,0)){

        input_len = ch_pp_din_rdlen(din);
        if(input_len<6){

            return 1; /*need more data*/
        }

        if(_ssh_packet_parse(ssh_pkt,din)){
            /* error */
            return -1;
        }

        if(ssh_pkt->msg_code == SSH_MSG_NEWKEYS){

            ssh_data->state = SSH_STATE_NKEY;
            return 0;
        }

        input_len = ch_pp_din_rdlen(din);

        if(input_len+2>=ssh_pkt->pkt_len){

            ch_pp_din_pos_update(din,ssh_pkt->pkt_len-2);
        }else{
            ch_pp_din_pos_update(din,-6);/* reset packet header ,wait more data */
            return 1;
        }
    }

    return 0;
}


static int _do_data_parse(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused, ch_tcp_session_t *tsession,
        void *data,size_t dlen,int is_req){

    int ret;
    int rc = PARSE_CONTINUE;
    ch_ssh_session_entry_t *ssh_entry = (ch_ssh_session_entry_t*)tsession->sentry;
    ch_pp_data_input_t *din = is_req?&ssh_entry->data_input_cli:&ssh_entry->data_input_srv;
    ch_ssh_data_t *ssh_data = is_req?&ssh_entry->cli_data:&ssh_entry->srv_data;

    ssh_data->pkts+=1;
    ssh_data->bytes += dlen;

    if(ssh_data->state == SSH_STATE_NKEY||ssh_data->state == SSH_STATE_ENC){

        ssh_data->enc_pkts += 1;
        ssh_data->enc_bytes += dlen;

        if(ssh_data->enc_minBytes == 0||ssh_data->enc_minBytes>dlen)
            ssh_data->enc_minBytes = dlen;

        if(ssh_data->enc_maxBytes == 0 ||ssh_data->enc_maxBytes<dlen)
            ssh_data->enc_maxBytes = dlen;

        return PARSE_CONTINUE;
    }

    if(data == NULL || dlen == 0)
        return PARSE_CONTINUE;

    ch_proto_session_entry_update(&ssh_entry->psEntry,dlen,is_req);


    if(ch_pp_din_read_prefare(din,data,dlen)){

		ch_log(CH_LOG_ERR,"Cannot init data input to parse ssh!");
		return PARSE_BREAK;

    }

    do {

        if(ssh_data->state == SSH_STATE_INIT){
            /* parse version */
            ret = _parse_ssh_version(ssh_entry,ssh_data,din);
            if(ret){
                rc = (ret == -1?PARSE_BREAK:PARSE_CONTINUE);
                break;
            }
        }

        if(ssh_data->state == SSH_STATE_VERSION){

            /* parse record */
            ret = _ssh_parse_record(ssh_entry,ssh_data,din);
            if(ret){
                rc = (ret == -1?PARSE_BREAK:PARSE_CONTINUE);
                break;
            }
        }

    }while(0);

	if(ch_pp_din_read_end(din)){
	
		ch_log(CH_LOG_ERR,"Cannot complete data input end for parse ssh!");
		return PARSE_BREAK ;
	}

    return rc;
}

static int 
do_ssh_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,void *data,size_t dlen){

    
    return _do_data_parse(app,pstore,tsession,data,dlen,1);

}


static int 
do_ssh_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,void *data,size_t dlen){

    return _do_data_parse(app,pstore,tsession,data,dlen,0);

}

