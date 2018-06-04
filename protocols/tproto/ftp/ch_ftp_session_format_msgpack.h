/*
 *
 *      Filename: protocols/tproto/ftp/ch_ftp_session_format_msgpack.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2017-01-06 10:09:46
 * Last Modified: 2017-01-06 10:09:46
 */


#ifndef __CH_FTP_SESSION_FORMAT_MSGPACK_H__
#define __CH_FTP_SESSION_FORMAT_MSGPACK_H__

#include "ch_session_format.h"

int
ch_ftp_session_format(ch_session_format_t *fmt, void *session_in, void *priv_data);

#endif //__CH_FTP_SESSION_FORMAT_MSGPACK_H__

