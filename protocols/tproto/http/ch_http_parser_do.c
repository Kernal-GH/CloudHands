/*
 *
 *      Filename: ch_http_parser_do.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-20 00:20:07
 * Last Modified: 2016-11-22 20:09:20
 */


static inline int _http_body_file_create(ch_http_session_t *session,ch_http_proto_t *hpproto,int is_req){

	ch_http_session_body_t *body;
	char *fname,*fname_tmp;
	ch_fpath_t *fpath = is_req?hpproto->req_fpath:hpproto->res_fpath;

	ch_fpath_name_create(fpath,&fname,&fname_tmp);
	
	if(fname == NULL){
	
		ch_log(CH_LOG_ERR,"Can not create a file used to store http body!");
		return -1;
	}

	body = ch_http_session_body_create(session->mp,(const char*)fname,0);

	if(body == NULL)
		return -1;

	if(is_req)
		session->req_body = body;
	else
		session->res_body = body;

	return 0;
}

static inline int _http_body_is_chunked(ch_http_session_t *session,int is_req){

	return is_req?session->headers_in.chunked:session->headers_out.chunked;
}

static inline void _http_body_parsed_end(ch_http_session_t *session,int is_req){

	if(is_req)
		session->parse_phase = PARSE_PHASE_STATUS_LINE; 
	else
		session->parse_phase = PARSE_PHASE_DONE;
}

static int _http_body_chunked_parse(ch_http_session_t *session,ch_buf_t *pb_ptr,int is_req){

	void *chunk_data = NULL;
	size_t chunk_data_len = 0;
	int rc = CH_AGAIN;
	size_t size;
	ch_http_chunked_t *chunk;
	ch_http_session_parse_data_t *pdata;

	ch_http_session_body_t *body = is_req?session->req_body:session->res_body;

	if(body->chunk == NULL){
	
		ch_http_session_body_chunk_init(body);
		if(body->chunk == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot allocate instance of body chunk!");
			return -1;
		}
	}

	chunk = body->chunk;
	chunk->pdata.pb = pb_ptr;
	pdata = &chunk->pdata;

	/*start to parse data chunked */
	for ( ;; ) {

		chunk_data = NULL;
		chunk_data_len = 0;

		rc = ch_http_chunked_parse(chunk);

		if (rc == CH_OK) {

			/* a chunk has been parsed successfully */
			size = pdata->pb->end - pdata->pb->pos;
			chunk_data = pdata->pb->pos;

			if (size > chunk->size) {

				chunk_data_len = chunk->size;

				pdata->pb->pos +=  chunk->size;
				chunk->size = 0;
			} else {

				chunk_data_len = size;
				chunk->size -= size;
				pdata->pb->pos = pdata->pb->end;
			}

			if(is_req)
				session->headers_in.content_length_n += chunk_data_len;
			else
				session->headers_out.content_length_n += chunk_data_len;

			ch_http_session_body_append(session,chunk_data,chunk_data_len,is_req);

			continue;
		}

		if (rc == CH_DONE) {

			/* a whole body has been parsed successfully */
			
			_http_body_parsed_end(session,is_req);
			break;
		}

		if (rc == CH_AGAIN) {

			/* amount of data we want to see next time */

			break;
		}

		/* invalid */

		ch_log(CH_LOG_ERR,"invalid chunked body");

		return -1;
	}

	return 0;
}

static inline int _is_http_body_complete(ch_http_session_t *session,int is_req){

	if(is_req)
		return session->req_body->body_len_accepted>=session->headers_in.content_length_n;

	return session->res_body->body_len_accepted>=session->headers_out.content_length_n;
}

static int _http_body_direct_append(ch_http_session_t *session,ch_buf_t *pb,int is_req){

	void *data;
	size_t dlen;

	if(pb->pos == pb->start){
	
		data = pb->start;
		dlen = (size_t)(pb->end-pb->start);
	}else{
	
		data = pb->pos;
		dlen = (size_t)(pb->end-pb->pos);
	}

	ch_http_session_body_append(session,data,dlen,is_req);

	if(_is_http_body_complete(session,is_req))
		_http_body_parsed_end(session,is_req);


	pb->pos +=dlen;

	return 0;
}

static inline int _is_need_create_file_body(ch_http_session_t *session,int is_req){

	if(is_req)
		return session->req_body == NULL;
	return session->res_body == NULL;
}

static int _http_body_parse(ch_app_proto_t *aproto,ch_http_session_t *session,ch_buf_t *pb,int is_req){

	ch_http_proto_t *hpproto = (ch_http_proto_t*)aproto;

	if(_is_need_create_file_body(session,is_req)){
	
		if(_http_body_file_create(session,hpproto,is_req)){
		
			ch_log(CH_LOG_ERR,"Create http  body file store failed!");
			return -1;
		}
	}

	if(_http_body_is_chunked(session,is_req))
		/* parse body chunked */
		return _http_body_chunked_parse(session,pb,is_req);
	
	return _http_body_direct_append(session,pb,is_req);

}

static int _http_header_data_remained_store(ch_http_session_entry_t *hsentry,ch_http_header_t *header,int is_req){

	void *data;
	size_t dlen = 0;

	ch_buf_t *pb = header->pdata.pb;

	if(header->header_name_start == NULL)
		return 0;

	data = header->header_name_start;
	dlen = (size_t)(pb->pos-data);

	return ch_http_session_entry_data_store(hsentry,data,dlen,is_req);
}


static inline int _http_has_body(ch_http_session_t *session,int is_req){

	if(is_req)
		return session->headers_in.content_length_n>0||session->headers_in.chunked;

	
	return session->headers_out.content_length_n>0||session->headers_out.chunked;
	
}

static void _http_header_parse_end(ch_http_session_t *session,int is_req){

	int has_body = _http_has_body(session,is_req);

	if(is_req)
		session->parse_phase = has_body?PARSE_PHASE_REQ_BODY:PARSE_PHASE_STATUS_LINE; 
	else
		session->parse_phase = has_body?PARSE_PHASE_RES_BODY:PARSE_PHASE_DONE;
}

static int _http_header_parse(ch_app_proto_t *aproto,ch_http_session_entry_t *hsentry,
	ch_http_session_t *session,ch_buf_t *pb,int is_req){

	/* unused */
	aproto = aproto;

	ch_http_header_t header,*hptr = &header;
	int rc = CH_AGAIN,rv;

	ch_http_header_init(hptr,pb);

    for ( ;; ) {

        rc = ch_http_header_line_parse(hptr);

        if (rc == CH_OK) {

            /* a header line has been parsed successfully */

			ch_http_session_header_add(session,hptr,is_req);

            continue;
        }

        if (rc == CH_HTTP_PARSE_HEADER_DONE) {

            /* a whole header has been parsed successfully */            
			_http_header_parse_end(session,is_req);
			ch_http_session_entry_data_store_reset(hsentry,is_req);

			break;
        }

        if (rc == CH_AGAIN) {

            /* a header line parsing is still not complete */
			rv = _http_header_data_remained_store(hsentry,hptr,is_req);
			if (rv) {
				ch_log(CH_LOG_ERR,"Cannot store remain data,when parsed header!");
				return -1;
			}

			break;

        }

        /* rc == CH_HTTP_PARSE_INVALID_HEADER */

        ch_log(CH_LOG_ERR,"invalid header line");
        return -1;
    }

	return 0;

}

static int _http_request_line_data_remained_store(ch_http_session_entry_t *hsentry,ch_http_request_line_t *line){

	void *data;
	size_t dlen = 0;

	ch_buf_t *pb = line->pdata.pb;

	if(line->request_start == NULL)
		return 0;

	data = line->request_start;

	dlen = (size_t)(pb->pos-data);

	return ch_http_session_entry_data_store(hsentry,data,dlen,1);
}

static inline void _http_request_line_parse_end(ch_http_session_t *session){

	if(session->http_version == CH_HTTP_VERSION_9){
	
		/*HTTP/0.9 no header and body */
		session->parse_phase = PARSE_PHASE_RES_BODY;
	}else{
	
		session->parse_phase = PARSE_PHASE_REQ_HEADER;
	}
}

static int _http_request_line_parse(ch_app_proto_t *aproto,ch_http_session_entry_t *hsentry,
	ch_http_session_t *session,ch_buf_t *pb){

	/* unused */
	aproto = aproto;
	
	ch_http_request_line_t line,*line_ptr = &line;
	int rc = CH_AGAIN;

	ch_http_request_line_init(line_ptr,pb);

	rc = ch_http_request_line_parse(line_ptr);

	if (rc == CH_OK) {

		/* the request line has been parsed successfully */
		ch_http_session_request_line_set(session,line_ptr);

		_http_request_line_parse_end(session);

		ch_http_session_entry_data_store_reset(hsentry,1);

		return 0;
	}

	if (rc != CH_AGAIN) {

		/* there was error while a request line parsing */

		ch_log(CH_LOG_ERR,"invalid http request line!");
		return -1;
	}

	/* CH_AGAIN: a request line parsing is still incomplete */

	if (pb->pos == pb->end) {
		
		if(_http_request_line_data_remained_store(hsentry,line_ptr)){

			ch_log(CH_LOG_ERR,"store http request line remain data failed!");
			return -1;
		}
	}

	return 0;

}

static int _http_status_line_parse(ch_app_proto_t *aproto,ch_http_session_entry_t *hsentry,
	ch_http_session_t *session,ch_buf_t *pb){

	/* unused */
	aproto = aproto;
	hsentry = hsentry;

	ch_http_status_line_t status,*status_line = &status;

	ch_http_status_line_init(status_line,pb);

	if(ch_http_status_line_parse(status_line)!=CH_OK){
	
		ch_log(CH_LOG_ERR,"Invalid http response status line!");

		return -1;
	}

	session->parse_phase = PARSE_PHASE_RES_HEADER;

	ch_http_session_status_line_set(session,status_line);

	return 0;
}

static inline int _is_buf_empty(ch_buf_t *pb){

	return pb == NULL || pb->pos == pb->end;
}

static int http_request_parse(ch_app_proto_t *aproto,ch_session_entry_t *sentry,void *data,uint32_t dlen,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	int rc;
	ch_buf_t pb,*pb_ptr = &pb;
	ch_http_session_t *session = NULL;
	ch_http_session_entry_t *hsentry = (ch_http_session_entry_t*)sentry->priv_data;
	

	ch_http_proto_t *hpproto = (ch_http_proto_t*)aproto;

	session = ch_http_sentry_session_get(hsentry,1);
	if(session == NULL){

		/* no memory space to allocate a new session,so drop this tentry */
		ch_log(CH_LOG_ERR,"No memory to allocate a new session,so drop this session!");
		return PARSE_BREAK; 
	}

	if(session->parse_phase>PARSE_PHASE_REQ_BODY){

		ch_log(CH_LOG_ERR,"Invalid parse phase status!");
		return PARSE_BREAK;
	}

	if(ch_http_proto_buf_init(hpproto,pb_ptr,hsentry->cur_req_buf,data,dlen)){
	
		ch_log(CH_LOG_ERR,"Cannot init parser buffer to parse http request!");
		return PARSE_BREAK;
	}
		
	/* start to parse */
	while(_is_buf_empty(pb_ptr)== 0){

		if(session->parse_phase>PARSE_PHASE_REQ_BODY){
		
			/*has a new http request,should store current session,and create a new session,then continue to parse */
			ch_http_sentry_session_add(hsentry,session);
			session = ch_http_sentry_session_get(hsentry,1);
			
			if(session == NULL){

				/* no memory space to allocate a new session,so drop this tentry */
				ch_log(CH_LOG_ERR,"No memory to allocate a new session,so drop this session!");
				return PARSE_BREAK; 
			}

		}

		switch(session->parse_phase){

		case PARSE_PHASE_REQ_LINE:
			/* parse http request line */
			rc = _http_request_line_parse(aproto,hsentry,session,pb_ptr);
			if(rc){
			
				/* error */
				ch_log(CH_LOG_ERR,"Parse http request line failed!");
				return PARSE_BREAK;
			}

			break;

		case PARSE_PHASE_REQ_HEADER:
			/* parse http headers */
			rc = _http_header_parse(aproto,hsentry,session,pb_ptr,1);
			if(rc){
			
				/* error */
				ch_log(CH_LOG_ERR,"Parse http request header failed!");
				return PARSE_BREAK;
			}
			break;

		case PARSE_PHASE_REQ_BODY:

			/*parse http request body */
			rc = _http_body_parse(aproto,session,pb_ptr,1);
			if(rc){
			
				/* error */
				ch_log(CH_LOG_ERR,"Parse http request body failed!");
				return PARSE_BREAK;
			}
			break;

		default:
			break;
		} //!switch
	} //!for(;;)
	
	if(session->parse_phase>PARSE_PHASE_REQ_BODY){
	
		/*should store current session*/
		ch_http_sentry_session_add(hsentry,session);
		
	}

	return PARSE_CONTINUE;
}


static void _http_session_format(ch_app_proto_t *aproto,ch_session_entry_t *sentry,ch_http_session_t *session){

	ch_http_session_entry_t *hsentry = (ch_http_session_entry_t*)sentry->priv_data;

	ch_http_sentry_session_remove(hsentry,session);

	ch_session_format(aproto->fmt,sentry,(void*)session,NULL);

}

static int 
http_response_parse(ch_app_proto_t *aproto,ch_session_entry_t *sentry,void *data,uint32_t dlen,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	int rc;
	ch_buf_t pb,*pb_ptr = &pb;
	ch_http_session_t *session = NULL;
	ch_http_session_entry_t *hsentry = (ch_http_session_entry_t*)sentry->priv_data;
	

	ch_http_proto_t *hpproto = (ch_http_proto_t*)aproto;

	session = ch_http_sentry_session_get(hsentry,0);

	if(session == NULL){

		/* no request??? */
		ch_log(CH_LOG_ERR,"Cannot find http request for this response data!");
		return PARSE_BREAK; 
	}

	if(session->parse_phase>PARSE_PHASE_DONE){

		ch_log(CH_LOG_ERR,"Invalid parse phase status!");
		return PARSE_BREAK;
	}

	if(ch_http_proto_buf_init(hpproto,pb_ptr,hsentry->cur_res_buf,data,dlen)){
	
		ch_log(CH_LOG_ERR,"Cannot init parser buffer to parse http response!");
		return PARSE_BREAK;
	}
		
	/* start to parse */
	while(_is_buf_empty(pb_ptr)== 0){

		if(session->parse_phase==PARSE_PHASE_DONE){
		
			/*has a new http respones,should output current session,and get a new session,then continue to parse */
		
			_http_session_format(aproto,sentry,session);

			session = ch_http_sentry_session_get(hsentry,0);
			
			if(session == NULL){

				/* no request??? */
				ch_log(CH_LOG_ERR,"Cannot find http request for this response data!");
				return PARSE_BREAK; 
			}

		}

		switch(session->parse_phase){

		case PARSE_PHASE_STATUS_LINE:
			/* parse http status line */
			rc = _http_status_line_parse(aproto,hsentry,session,pb_ptr);
			if(rc){
			
				/* error */
				ch_log(CH_LOG_ERR,"Parse http response status line failed!");
				return PARSE_BREAK;
			}
			break;

		case PARSE_PHASE_RES_HEADER:
			/* parse http response headers */
			rc = _http_header_parse(aproto,hsentry,session,pb_ptr,0);
			if(rc){
			
				/* error */
				ch_log(CH_LOG_ERR,"Parse http response header failed!");
				return PARSE_BREAK;
			}

			break;

		case PARSE_PHASE_RES_BODY:

			/*parse http response body */
			rc = _http_body_parse(aproto,session,pb_ptr,0);
			if(rc){
			
				/* error */
				ch_log(CH_LOG_ERR,"Parse http response body failed!");
				return PARSE_BREAK;
			}

			break;

		default:
			break;
		} //!switch
	} //!for(;;)
	
	if(session->parse_phase==PARSE_PHASE_DONE){
	
		/*should format session*/
		_http_session_format(aproto,sentry,session);

		if(is_http_session_keepalive(session) == 0){
		
			return PARSE_DONE;
		}
	}

	return PARSE_CONTINUE;

}

