/*
 *
 *      Filename: do_http_format.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-17 12:04:21
 * Last Modified: 2018-05-17 19:23:21
 */

#define BODY_NAME(body) ((body)==NULL?"":(body)->fname)

static inline void _pack_headers(msgpack_packer *pk,struct list_head *h,const char *k,unsigned int n){

	ch_table_elt_t *header;
	ch_msgpack_map_start(pk,k,n);

	list_for_each_entry(header,h,node){
	
		ch_msgpack_write_kv(pk,(const char*)header->key,(const char*)header->val);
	}
}

static void do_http_session_format(msgpack_packer *pk,void *session_in){

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
	ch_msgpack_write_kv(pk,"reqBodyPath",BODY_NAME(session->req_body));
	
	/*pack response body path */
	ch_msgpack_write_kv(pk,"resBodyPath",BODY_NAME(session->res_body));

	/*pack request headers */

	_pack_headers(pk,&session->headers_in.headers,"inHeaders",session->in_headers_n);

	/*pack response headers */
	_pack_headers(pk,&session->headers_out.headers,"outHeaders",session->out_headers_n);

}


