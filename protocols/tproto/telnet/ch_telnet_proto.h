/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_proto.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 11:55:31
 * Last Modified: 2016-12-09 11:55:31
 */


#ifndef __CH_TELNET_PROTO_H__
#define __CH_TELNET_PROTO_H__

#include "ch_telnet_context.h"

typedef struct ch_telnet_proto_s {
	ch_app_proto_t aproto;

	ch_telnet_context_t *tcontext;

	ch_fpath_t *trans_fpath;
} ch_telnet_proto_t;

ch_app_proto_t *ch_telnet_proto_create(apr_pool_t *mp, ch_proto_context_t *pcontext);

#endif //__CH_TELNET_PROTO_H__

