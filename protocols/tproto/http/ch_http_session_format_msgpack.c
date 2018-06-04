/*
 *
 *      Filename: ch_http_session_format_msgpack.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-20 00:09:30
 * Last Modified: 2016-10-28 01:53:33
 */

static inline const char * _body_name(ch_http_session_body_t *body){

	if(body == NULL)
		return "";
	return body->fname;
}

static inline void _pack_headers(msgpack_packer *pk,struct ch_list_head *h,const char *k,unsigned int n){

	ch_table_elt_t *header;
	ch_msgpack_map_start(pk,k,n);

	list_for_each_entry(header,h,node){
	
		ch_msgpack_write_kv(pk,(const char*)header->key,(const char*)header->val);
	}
}

static int
http_session_format(ch_session_format_t *fmt,void *session_in,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;
	msgpack_packer *pk = &msgpack_fmt->pk;

	ch_http_session_t * session = (ch_http_session_t*)session_in;

	ch_msgpack_map_start(pk,"http",8);
	
	/*pack method */
	ch_msgpack_write_kv(pk,"method",(const char*)session->method);

	/*pack uri */
	ch_msgpack_write_kv(pk,"uri",(const char*)session->uri);

	/*pack http_version */
	ch_msgpack_write_kv(pk,"version",ch_http_version_to_str(session->http_version));

	/*pack response status code */
	ch_msgpack_write_uint(pk,"status",session->status_code);

	/*pack request body path*/
	ch_msgpack_write_kv(pk,"reqBodyPath",_body_name(session->req_body));
	
	/*pack response body path */
	ch_msgpack_write_kv(pk,"resBodyPath",_body_name(session->res_body));

	/*pack request headers */

	_pack_headers(pk,&session->headers_in.headers,"inHeaders",session->in_headers_n);

	/*pack response headers */
	_pack_headers(pk,&session->headers_out.headers,"outHeaders",session->out_headers_n);

	return 0;
}

