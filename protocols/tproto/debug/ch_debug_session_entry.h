/*
 *
 *      Filename: ch_debug_session_entry.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 22:30:12
 * Last Modified: 2016-12-05 22:30:12
 */

#ifndef CH_DEBUG_SESSION_ENTRY_H
#define CH_DEBUG_SESSION_ENTRY_H

typedef struct ch_debug_session_entry_t ch_debug_session_entry_t;

#include <apr_pools.h>
#include "ch_session_entry.h"

struct ch_debug_session_entry_t {

	apr_pool_t *mp;
	FILE *req_fp;
	FILE *res_fp;

	const char *req_fpath;
	const char *res_fpath;
};

extern void ch_debug_sentry_init(ch_debug_session_entry_t *dsentry,ch_session_entry_t *sentry,
	ch_mmap_file_entry_t *fentry,apr_pool_t *mp);

extern void ch_debug_sentry_data_write(ch_debug_session_entry_t *dsentry,void *data,size_t dlen,int is_req);


extern int ch_debug_sentry_fpath_init(ch_debug_session_entry_t *dsentry,const char *fpath,int is_req);

extern void ch_debug_session_entry_fin(ch_debug_session_entry_t *dsentry);

#endif /* CH_DEBUG_SESSION_ENTRY_H */
