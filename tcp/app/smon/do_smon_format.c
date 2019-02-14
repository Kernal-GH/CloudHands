/*
 *
 *      Filename: do_smon_format.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-17 12:04:21
 * Last Modified: 2018-07-17 16:53:16
 */

#define SMON_BODY_NAME(body) ((body)==NULL?"":body)

void do_smon_session_format(msgpack_packer *pk,void *session_in){

	ch_smon_session_entry_t * session = (ch_smon_session_entry_t*)session_in;


	ch_msgpack_map_start(pk,"smon",3);

	ch_msgpack_write_uint64(pk,"id",session->id);

	/*pack request body path*/
	ch_msgpack_write_kv(pk,"reqBodyPath",SMON_BODY_NAME(session->req_content_fpath));
	
	/*pack response body path */
	ch_msgpack_write_kv(pk,"resBodyPath",SMON_BODY_NAME(session->res_content_fpath));


}


