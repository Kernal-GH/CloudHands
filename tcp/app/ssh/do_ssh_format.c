/*
 * =====================================================================================
 *
 *       Filename:  do_ssh_format.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/17/2018 01:57:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

static inline  void ssh_data_format(ch_ssh_data_t *ssh_data,msgpack_packer *pk,const char *key){

	ch_msgpack_map_start(pk,key,9);

    ch_msgpack_write_uint8(pk,"state",ssh_data->state);
    ch_msgpack_write_uint8(pk,"version",ssh_data->version);

	ch_msgpack_write_kv(pk,"pversion",ssh_data->proto_version);

	ch_msgpack_write_uint64(pk,"pkts",ssh_data->pkts);
	ch_msgpack_write_uint64(pk,"bytes",ssh_data->bytes);
	ch_msgpack_write_uint64(pk,"encPkts",ssh_data->enc_pkts);
	ch_msgpack_write_uint64(pk,"encBytes",ssh_data->enc_bytes);
	ch_msgpack_write_uint64(pk,"encMinBytes",ssh_data->enc_minBytes);
	ch_msgpack_write_uint64(pk,"encMaxBytes",ssh_data->enc_maxBytes);

}

static void do_ssh_session_format(msgpack_packer *pk,void *session_in){


	ch_ssh_session_entry_t * session = (ch_ssh_session_entry_t*)session_in;


	ch_msgpack_map_start(pk,"ssh",2);

    ssh_data_format(&session->cli_data,pk,"request");
    ssh_data_format(&session->srv_data,pk,"response");
}

