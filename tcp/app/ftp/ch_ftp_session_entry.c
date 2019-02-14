/*
 *
 *      Filename: ch_ftp_session_entry.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-21 11:26:14
 * Last Modified: 2018-09-21 15:47:31
 */

#include "ch_ftp_session_entry.h"

void ch_ftp_session_entry_init(ch_ftp_session_entry_t *ftp_entry,ch_pool_t *mp,
	ch_ftp_session_t *ftp_session,ch_buffer_t *g_buffer){


	ch_proto_session_entry_init(&ftp_entry->psEntry);

	ftp_entry->mp = mp;

	ftp_entry->ftp_session = ftp_session;

	ch_pp_din_init(&ftp_entry->data_input_req,g_buffer,mp); 
	ch_pp_din_init(&ftp_entry->data_input_res,g_buffer,mp); 

}
