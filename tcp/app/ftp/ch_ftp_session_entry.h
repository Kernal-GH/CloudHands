/*
 *
 *      Filename: ch_ftp_session_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 11:20:57
 * Last Modified: 2018-09-21 11:20:57
 */

#ifndef CH_FTP_SESSION_ENTRY_H
#define CH_FTP_SESSION_ENTRY_H

#include "ch_ftp_session.h"
#include "ch_mpool.h"
#include "ch_pp_data_input.h"
#include "ch_proto_session_entry.h"

typedef struct ch_ftp_session_entry_t ch_ftp_session_entry_t;

struct ch_ftp_session_entry_t {

	ch_proto_session_entry_t psEntry;
	
	ch_pp_data_input_t data_input_req;
	ch_pp_data_input_t data_input_res;

	ch_pool_t *mp;

	ch_ftp_session_t *ftp_session;


};

extern void ch_ftp_session_entry_init(ch_ftp_session_entry_t *ftp_entry,ch_pool_t *mp,
	ch_ftp_session_t *ftp_session,ch_buffer_t *g_buffer);

#endif /*CH_FTP_SESSION_ENTRY_H*/
