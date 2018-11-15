/*
 *
 *      Filename: ch_http_session.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-21 23:58:20
 * Last Modified: 2018-09-10 11:26:56
 */

#include "ch_log.h"
#include "ch_http_session.h"

static inline void _http_headers_in_init(ch_http_headers_in_t *hin){

	INIT_LIST_HEAD(&hin->headers);
	hin->host = NULL;
	hin->connection = NULL;
	hin->user_agent = NULL;
	hin->content_length = NULL;
	hin->content_type = NULL;
	hin->transfer_encoding = NULL;
	hin->content_length_n = 0;
	hin->connection_type = CH_HTTP_CONNECTION_CLOSE;
	hin->chunked = 0;
}

static inline void _http_headers_out_init(ch_http_headers_out_t *hout){

	INIT_LIST_HEAD(&hout->headers);
	hout->server = NULL;
	hout->connection = NULL;
	hout->content_length = NULL;
	hout->content_encoding = NULL;
	hout->transfer_encoding = NULL;
	hout->content_type = NULL;
	hout->content_length_n = 0;
	
	hout->connection_type = CH_HTTP_CONNECTION_CLOSE;
	hout->chunked = 0;
}

ch_http_session_t * ch_http_session_create(ch_pool_t *mp){

	ch_http_session_t *session = (ch_http_session_t*)ch_palloc(mp,sizeof(ch_http_session_t));

	if(session == NULL){
	
		ch_log(CH_LOG_ERR,"create http session instance failed,no memory !");

		return NULL;
	}

	ch_proto_session_entry_init(&session->psEntry);

	_http_headers_in_init(&session->headers_in);
	_http_headers_out_init(&session->headers_out);

	session->mp = mp;
	session->req_body = NULL;
	session->res_body = NULL;
	session->parse_phase = PARSE_PHASE_REQ_LINE;
	session->http_version = CH_HTTP_VERSION_11;
	session->method = NULL;
	session->uri = NULL;
	session->host = NULL;
	session->in_headers_n = 0;
	session->out_headers_n = 0;
	session->status_code = 0;

	return session;
}


void ch_http_session_request_line_set(ch_http_session_t *session,ch_http_request_line_t *req_line){

	/*set method*/
	HTTP_STRNDUP(session->mp,session->method,req_line->request_start,req_line->method_end,1);

	/* set http version */
	session->http_version = HTTP_VERSION_MAKE(req_line->http_major,req_line->http_minor);

	/*set uri */
	HTTP_STRNDUP(session->mp,session->uri,req_line->uri_start,req_line->uri_end,0);

}

static void _http_request_header_index_set(ch_http_session_t *session,ch_table_elt_t *header){

    size_t len = strlen((const char*)header->key);
    switch(len){

        case 4:
            
            if(strcasecmp((const char*)header->key,"Host") == 0){
                session->headers_in.host = header;
				session->host = header->val;
            }

            break;

        case 10:

            if(strcasecmp((const char*)header->key,"Connection") == 0){
                session->headers_in.connection = header;
				
				if (strncasecmp((const char*)header->val, "close", 5 - 1)) {
					session->headers_in.connection_type = CH_HTTP_CONNECTION_CLOSE;

				} else if (strncasecmp((const char*)header->val, "keep-alive", 10 - 1)) {
					session->headers_in.connection_type = CH_HTTP_CONNECTION_KEEP_ALIVE;
				}

            }
            
            else if(strcasecmp((const char*)header->key,"User-Agent") == 0){
                session->headers_in.user_agent = header;
            }

            break;

        case 12:
            
            if(strcasecmp((const char*)header->key,"Content-Type") == 0){
                session->headers_in.content_type = header;
            }
            
            break;

        case 14:
            
            if(strcasecmp((const char*)header->key,"Content-Length") == 0){
                session->headers_in.content_length = header;
				STRTOSIZE(header->val,session->headers_in.content_length_n);
            }

            break;

        case 17:

            if(strcasecmp((const char*)header->key,"Transfer-Encoding") == 0){
                session->headers_in.transfer_encoding = header;
				if(strncasecmp((const char*)header->val,"chunked", 7)==0){
				
					session->headers_in.chunked = 1;
				}
            }

            break;

        default:
            break;
    }
}

static void _http_response_header_index_set(ch_http_session_t *session,ch_table_elt_t *header){

    size_t len = strlen((const char*)header->key);
    
    switch(len){

        case 6:

            if(strcasecmp((const char*)header->key,"Server") == 0){
                session->headers_out.server = header;
            }

            break;

        case 10:

            if(strcasecmp((const char*)header->key,"Connection") == 0){
                session->headers_out.connection = header;
				
				if (strncasecmp((const char*)header->val, "close", 5 - 1)) {
					session->headers_out.connection_type = CH_HTTP_CONNECTION_CLOSE;

				} else if (strncasecmp((const char*)header->val, "keep-alive", 10 - 1)) {
					session->headers_out.connection_type = CH_HTTP_CONNECTION_KEEP_ALIVE;
				}

            }

			break;

        case 12:
            
            if(strcasecmp((const char*)header->key,"Content-Type") == 0){
                session->headers_out.content_type = header;
            }
            
            break;

        case 14:
            
            if(strcasecmp((const char*)header->key,"Content-Length") == 0){
                session->headers_out.content_length = header;
				STRTOSIZE(header->val,session->headers_out.content_length_n);
            }

            break;

        case 16:

            if(strcasecmp((const char*)header->key,"Content-Encoding") == 0){
                session->headers_out.content_encoding = header;
            }

            break;

        case 17:
            
            if(strcasecmp((const char*)header->key,"Transfer-Encoding") == 0){
                session->headers_out.transfer_encoding = header;
				
				if(strncasecmp((const char*)header->val,"chunked", 7)==0){
				
					session->headers_out.chunked = 1;
				}
            }

            break;
        default:
            break;
    }
}
void ch_http_session_header_add(ch_http_session_t *session,ch_http_header_t *header,int is_req){

	ch_table_elt_t *entry = (ch_table_elt_t*)ch_palloc(session->mp,sizeof(ch_table_elt_t));

	HTTP_STRNDUP(session->mp,entry->key,header->header_name_start,header->header_name_end,0);
	HTTP_STRNDUP(session->mp,entry->val,header->header_start,header->header_end,0);

	if(is_req){
	
		list_add(&entry->node,&session->headers_in.headers);
		_http_request_header_index_set(session,entry);
		session->in_headers_n+=1;
	}else{	
		list_add(&entry->node,&session->headers_out.headers);
		_http_response_header_index_set(session,entry);
		session->out_headers_n+=1;
	}
}

void ch_http_session_status_line_set(ch_http_session_t *session,ch_http_status_line_t *status){

	session->status_code = status->code;
}

void ch_http_session_body_append(ch_http_session_t *session,unsigned char *data,size_t dlen,int is_req){

	if(is_req){
	
		ch_http_body_append(session->req_body,data,dlen);
	}else{
	
		ch_http_body_append(session->res_body,data,dlen);
	}
}
