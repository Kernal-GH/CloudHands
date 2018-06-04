/*
 *
 *      Filename: protocols/tproto/ftp/ch_ftp_session_format_msgpack.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-06 10:11:41
 * Last Modified: 2017-01-11 11:49:27
 */


#include <stdio.h>
#include <stdlib.h>
#include <msgpack.h>

#include "ch_log.h"
#include "ch_ftp_session.h"
#include "ch_session_entry.h"
#include "ch_session_format.h"
#include "ch_session_format_msgpack.h"
#include "ch_ftp_interactive_content_data.h"

int 
ch_ftp_session_format(ch_session_format_t *fmt, void *session_in, void *priv_data)
{
	ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;
	msgpack_packer *pk = &msgpack_fmt->pk;
	ch_ftp_session_t *session = (ch_ftp_session_t *)session_in;
	ch_ftp_cmds_data_t *cmds_data = session->cmds_data;

	if (session == NULL) {
		CH_ERR_LOG("%s: session is NULL or fpproto is NULL.\n", __func__);
		return -1;
	}

	priv_data = priv_data;

	ch_msgpack_map_start(pk, "FTP", 3);

	ch_msgpack_write_kv(pk, "username",
		session->user.data != NULL ? (const char *) session->user.data : "");

	ch_msgpack_write_kv(pk, "password",
		session->passwd.data != NULL ? (const char *) session->passwd.data : "");

	ch_msgpack_write_kv(pk, "interactive_data", cmds_data ? cmds_data->fname: "");

	return 0;
}

