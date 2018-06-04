/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session_format_msgpack.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-13 17:41:37
 * Last Modified: 2016-12-13 17:41:37
 */


#ifndef __CH_TELNET_SESSION_FORMAT_MSGPACK_H__
#define __CH_TELNET_SESSION_FORMAT_MSGPACK_H__

#include "ch_session_format.h"

int
ch_telnet_session_format(ch_session_format_t *fmt, void *session_in, void *priv_data);

#endif //__CH_TELNET_SESSION_FORMAT_MSGPACK_H__

