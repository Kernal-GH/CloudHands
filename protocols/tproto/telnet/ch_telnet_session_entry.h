/*
 *
 *      Filename: ./protocols/tproto/telnet/ch_telnet_session_entry.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-09 18:34:58
 * Last Modified: 2016-12-09 18:34:58
 */

#ifndef CH_TELNET_SESSION_ENTRY_H__
#define CH_TELNET_SESSION_ENTRY_H__

#include "ch_list.h"
#include "apr_pools.h"
#include "ch_telnet_proto.h"
#include "ch_session_entry.h"
#include "ch_telnet_session.h"
#include "ch_mmap_file_format.h"

typedef struct ch_telnet_session_entry_s {
	ch_session_entry_t *sentry;

	ch_telnet_proto_t *tpproto;

	apr_pool_t *mp;

	ch_telnet_session_t *cur_session;
} ch_telnet_session_entry_t;

void ch_telnet_sentry_init(ch_telnet_session_entry_t *tsentry,ch_session_entry_t *sentry, 
	ch_mmap_file_entry_t *fentry, apr_pool_t *mp, ch_telnet_proto_t *mpproto);

static inline void 
ch_telnet_sentry_session_remove(ch_telnet_session_entry_t *tsentry,
	ch_telnet_session_t *session)
{
	if (session == tsentry->cur_session) {
		tsentry->cur_session = NULL;
	}
}

#endif //CH_TELNET_SESSION_ENTRY_H__

