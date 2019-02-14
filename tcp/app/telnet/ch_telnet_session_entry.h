/*
 * =====================================================================================
 *
 *       Filename:  ch_telnet_session_entry.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2018 02:09:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_TELNET_SESSION_ENTRY_H
#define CH_TELNET_SESSION_ENTRY_H

typedef struct ch_telnet_session_entry_t ch_telnet_session_entry_t;

#include "ch_mpool.h"
#include "ch_proto_session_entry.h"

struct ch_telnet_session_entry_t {

	ch_proto_session_entry_t psEntry;

	ch_pool_t *mp;

    const char *user;
    const char *passwd;

	const char *content_fpath;

	FILE *content_fp;

};

extern void ch_telnet_session_entry_init(ch_telnet_session_entry_t *telnet_entry,ch_pool_t *mp);

extern int  ch_telnet_session_entry_fpath_init(ch_telnet_session_entry_t *telnet_entry,const char *fpath);

extern void ch_telnet_session_entry_write(ch_telnet_session_entry_t *telnet_entry,void *data,size_t dlen);

extern void ch_telnet_session_entry_fin(ch_telnet_session_entry_t *telnet_entry);

#endif /* CH_TELNET_SESSION_ENTRY_H */
