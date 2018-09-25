/*
 *
 *      Filename: do_ftp_data_session_format.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 11:11:37
 * Last Modified: 2018-09-25 13:50:22
 */


#define FTP_DATA_STR_VALUE(v) ((v)==NULL?"":v)

static void do_ftp_data_session_format(msgpack_packer *pk,void *session_in){


	ch_ftp_data_session_entry_t *session = (ch_ftp_data_session_entry_t*)session_in;

	ch_msgpack_map_start(pk,"ftpData",3);
	 
	ch_msgpack_write_kv(pk,"ftpPWDDir",FTP_DATA_STR_VALUE(session->ftp_path));
	ch_msgpack_write_kv(pk,"ftpFName",FTP_DATA_STR_VALUE(session->ftp_fname));
	ch_msgpack_write_kv(pk,"storePath",FTP_DATA_STR_VALUE(session->fstore_path));

}

