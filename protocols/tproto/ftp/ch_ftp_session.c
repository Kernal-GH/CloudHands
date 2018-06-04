/*
 *
 *      Filename: protocols/tproto/ftp/ch_ftp_session.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-05 11:36:46
 * Last Modified: 2017-01-10 17:07:34
 */


#include <stdio.h>
#include <stdlib.h>

#include <apr_pools.h>

#include "ch_log.h"
#include "ch_ftp_session.h"

static void ch_ftp_session_init(ch_ftp_session_t *s)
{
	s->user.data = NULL;
	s->user.len = 0;

	s->passwd.data = NULL;
	s->passwd.len = 0;

	s->cmds_data = NULL;
}

ch_ftp_session_t *ch_ftp_session_create(apr_pool_t *mp)
{
	ch_ftp_session_t *s = NULL;

	s = (ch_ftp_session_t *) apr_palloc(mp, sizeof(*s));
	if (s == NULL) {
		CH_ERR_LOG("%s: apr_palloc failed.\n", __func__);
		return NULL;
	}

	ch_ftp_session_init(s);

	return s;
}

void ch_ftp_session_destroy(ch_ftp_session_t *session)
{
	session = session;
	return;
}
