/*
 * =====================================================================================
 *
 *       Filename:  do_telnet_parse.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2018 05:42:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */


static int skip_command(unsigned char *ptr, unsigned char *end)
{
    int len;
    
    len = 0;
    while (ptr < end && *ptr == 0xff) {
        /* sub option 0xff 0xfa ... ... 0xff 0xf0 */
        if (*(ptr + 1) == 0xfa) {
            ptr += 1;
            len++;
            /* search the sub-option end (0xff 0xf0) */
            do {
                ptr += 1;
                len++;
            } while(*ptr != 0xff && ptr < end);
            /* skip the sub-option end */
            ptr += 2;
            len += 2;
        }
        else {
            /* normal option 0xff 0xXX 0xXX */
            ptr += 3;
            len += 3;
        }
    }

    return len;
}

/* 
 * convert 0x00 char into spaces (0x20) so we can
 * use str*() functions on the buffer...
 */
static void convert_zeros(unsigned char *ptr, unsigned char *end)
{
    int i;

    /* 
     * walk the entire buffer, but skip the last
     * char, if it is 0x00 it is actually the string
     * terminator
     */
    i = 0;
    while (ptr < end) {
        if (*ptr == 0x00) {
            /* convert the char to a space */
            if (i && *(ptr - 1) == '\r') {
                *ptr = '\n';
            }
            else {
                *ptr = ' ';
            }
        }
        ptr++;
        i = 1;
    }
}




#define TELNET_NEED_CREATE_FILE_BODY(session) (session->content_fp == NULL)

static inline int _do_content_process(ch_tcp_app_t *app,ch_proto_session_store_t *pstore, ch_tcp_session_t *tsession,
        void *data,size_t dlen,int is_req) {

	char *fname,*fname_tmp;
    size_t offset;

    if(data == NULL ||dlen ==0)
        return 0;

    private_telnet_context_t *mcontext = (private_telnet_context_t*)app->context;
    
    ch_proto_body_store_t *bstore = ch_proto_store_body_find_create(pstore,mcontext->content_dir,NULL,
            mcontext->create_dir_type,app->protocol_id); 

    if(bstore == NULL){
    
        return -1;
    }

	ch_telnet_session_entry_t *telnet_entry = (ch_telnet_session_entry_t*)tsession->sentry;

	ch_fpath_t *fpath = bstore->req_fpath;

	ch_proto_session_entry_update(&telnet_entry->psEntry,dlen,is_req);

	if(TELNET_NEED_CREATE_FILE_BODY(telnet_entry)){
	
		ch_fpath_name_create(fpath,&fname,&fname_tmp);
	
		if(fname == NULL){
		
			ch_log(CH_LOG_ERR,"Can not create a file used to store telnet body!");
			return -1;
		}

		if(ch_telnet_session_entry_fpath_init(telnet_entry,fname)){
		
			ch_log(CH_LOG_ERR,"Can not open a file used to store telnet body,fname:%s",fname);
			return -1;

		}
	}

    offset = skip_command((unsigned char*)data,(unsigned char*)(data+dlen));

    if(offset<dlen){
        
        convert_zeros((unsigned char*)(data+offset),(unsigned char*)(data+dlen));

        ch_telnet_session_entry_write(telnet_entry,data+offset,dlen-offset);

    }

	return 0;
}

static int 
do_telnet_request_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
        ch_tcp_session_t *tsession,void *data,size_t dlen){

	if(_do_content_process(app,pstore,tsession,data,dlen,1))
		return PARSE_BREAK;

	return PARSE_CONTINUE;
}


static int 
do_telnet_response_parse(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,
        ch_tcp_session_t *tsession,void *data,size_t dlen){

	if(_do_content_process(app,pstore,tsession,data,dlen,0))
		return PARSE_BREAK;

	return PARSE_CONTINUE;
}


