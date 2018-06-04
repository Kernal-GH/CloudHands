/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session_entry.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-13 17:13:01
 * Last Modified: 2016-12-30 17:55:15
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_telnet_session_entry.h"

void ch_telnet_sentry_init(ch_telnet_session_entry_t *tsentry,ch_session_entry_t *sentry, 
	ch_mmap_file_entry_t *fentry, apr_pool_t *mp, ch_telnet_proto_t *tpproto)
{
	ch_session_entry_init(sentry, fentry);

	tsentry->sentry = sentry;
	tsentry->tpproto = tpproto;
	tsentry->mp = mp;

	tsentry->cur_session = NULL;

	sentry->priv_data = (void *) tsentry;
}
