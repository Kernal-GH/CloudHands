/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-13 10:57:17
 * Last Modified: 2016-12-14 17:29:15
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_log.h"
#include "ch_telnet_session.h"

static void ch_telnet_session_init(ch_telnet_session_t *s)
{
	s->svr_state = TS_NORMAL;
	s->cli_state = TS_NORMAL;

	s->trans_data = NULL;
}

ch_telnet_session_t * ch_telnet_session_create(apr_pool_t *mp)
{
	ch_telnet_session_t *s = NULL;

	s = (ch_telnet_session_t *) apr_palloc(mp, sizeof(*s));
	if (s == NULL) {
		CH_ERR_LOG("%s: apr_palloc failed.\n", __func__);
		return NULL;
	}

	ch_telnet_session_init(s);

	return s;
}

void ch_telnet_session_destroy(ch_telnet_session_t *session)
{
	return;
}

