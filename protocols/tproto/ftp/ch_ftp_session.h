/*
 *
 *      Filename: protocols/tproto/ftp/ch_ftp_session.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-04 13:23:00
 * Last Modified: 2017-01-04 13:23:00
 */


#ifndef __CH_FTP_SESSION_H__
#define __CH_FTP_SESSION_H__

#include "ch_string.h"
#include "ch_ftp_interactive_content_data.h"

typedef struct ch_ftp_session_s {
	ch_str_t user;
	ch_str_t passwd;

	ch_ftp_cmds_data_t *cmds_data;
} ch_ftp_session_t;

ch_ftp_session_t *ch_ftp_session_create(apr_pool_t *mp);

void ch_ftp_session_destroy(ch_ftp_session_t *session);

#endif //__CH_FTP_SESSION_H__

