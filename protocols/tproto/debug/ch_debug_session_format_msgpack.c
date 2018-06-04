/*
 *
 *      Filename: ch_debug_session_format_msgpack.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 23:03:07
 * Last Modified: 2016-12-05 23:09:33
 */

static int
debug_session_format(ch_session_format_t *fmt,void *session_in,void *priv_data){

	/*unused*/
	priv_data = priv_data;

	ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;
	msgpack_packer *pk = &msgpack_fmt->pk;

	ch_debug_session_entry_t * session = (ch_debug_session_entry_t*)session_in;

	ch_msgpack_map_start(pk,"debug",2);
	
	/*pack request body path*/
	ch_msgpack_write_kv(pk,"reqBodyPath",session->req_fpath);
	
	/*pack response body path */
	ch_msgpack_write_kv(pk,"resBodyPath",session->res_fpath);

	return 0;
}

