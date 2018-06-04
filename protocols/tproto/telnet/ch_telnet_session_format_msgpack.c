/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session_format_msgpack.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-13 17:47:43
 * Last Modified: 2017-01-20 11:12:10
 */


#include <stdio.h>
#include <stdlib.h>
#include <msgpack.h>

#include "ch_log.h"
#include "ch_session_entry.h"
#include "ch_telnet_session.h"
#include "ch_telnet_transfer_data.h"
#include "ch_session_format_msgpack.h"
#include "ch_telnet_session_format_msgpack.h"

int 
ch_telnet_session_format(ch_session_format_t *fmt, void *session_in, void *priv_data)
{
	ch_session_format_msgpack_t *msgpack_fmt = (ch_session_format_msgpack_t*)fmt;
	msgpack_packer *pk = &msgpack_fmt->pk;
	ch_telnet_session_t *session = (ch_telnet_session_t *)session_in;
	ch_session_entry_t *sentry = (ch_session_entry_t *) priv_data;
	ch_telnet_transfer_data_t *trans_data = session->trans_data;

	if (session == NULL) {
		CH_ERR_LOG("%s: session is NULL or tpproto is NULL.\n", __func__);
		return -1;
	}

	ch_msgpack_map_start(pk, "TELNET", 1);

	ch_msgpack_write_kv(pk, "transfer_data", trans_data ? trans_data->fname: "");

	return 0;
}

