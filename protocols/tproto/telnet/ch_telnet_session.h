/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-13 10:39:23
 * Last Modified: 2016-12-13 10:39:23
 */


#ifndef __CH_TELNET_SESSION_H__
#define __CH_TELNET_SESSION_H__

#include "ch_telnet_transfer_data.h"

enum {
	TS_NORMAL = 0,
	TS_COPY = 1,
	TS_IAC = 2,
	TS_OPT = 3,
	TS_SUB1 = 4,
	TS_SUB2 = 5,
	TS_CR = 6,
};

typedef struct ch_telnet_session_s {
	unsigned char svr_state; /* telnet state for server(datas from server) */
	unsigned char cli_state; /* telnet state for client(datas from client) */

	ch_telnet_transfer_data_t *trans_data;
} ch_telnet_session_t;

ch_telnet_session_t *ch_telnet_session_create(apr_pool_t *mp);

void ch_telnet_session_destroy(ch_telnet_session_t *session);

#endif //__CH_TELNET_SESSION_H__
