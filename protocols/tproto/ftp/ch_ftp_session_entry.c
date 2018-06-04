/*
 *
 *      Filename: protocols/tproto/ftp/ch_ftp_session_entry.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-06 10:16:53
 * Last Modified: 2017-01-10 17:09:21
 */


#include <stdio.h>
#include <stdlib.h>

#include <apr_pools.h>

#include "ch_ftp_proto.h"
#include "ch_ftp_session_entry.h"
#include "ch_session_entry.h"
#include "ch_mmap_file_format.h"

void ch_ftp_sentry_init(ch_ftp_session_entry_t *fsentry,ch_session_entry_t *sentry, 
	ch_mmap_file_entry_t *fentry, apr_pool_t *mp, ch_ftp_proto_t *fpproto)
{
	ch_session_entry_init(sentry, fentry);

	fsentry->sentry = sentry;
	fsentry->fpproto = fpproto;

	fsentry->mp = mp;

	ch_ftp_tx_buffer_init(&fsentry->cur_req_buf);
	ch_ftp_tx_buffer_init(&fsentry->cur_res_buf);

	fsentry->session = NULL;

	sentry->priv_data = (void *) fsentry;
}
