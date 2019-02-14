/*
 *
 *      Filename: ch_smon_session_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-13 12:01:52
 * Last Modified: 2018-07-13 12:01:52
 */

#ifndef CH_SMON_SESSION_ENTRY_H
#define CH_SMON_SESSION_ENTRY_H

typedef struct ch_smon_session_entry_t ch_smon_session_entry_t;

#include "ch_mpool.h"
#include "ch_proto_session_entry.h"

struct ch_smon_session_entry_t {

	ch_proto_session_entry_t psEntry;

	ch_pool_t *mp;

	uint64_t id;

	const char *req_content_fpath;
	const char *res_content_fpath;

	FILE *req_content_fp;
	FILE *res_content_fp;

};

extern void ch_smon_session_entry_init(ch_smon_session_entry_t *smon_entry,ch_pool_t *mp);

extern int  ch_smon_session_entry_fpath_init(ch_smon_session_entry_t *smon_entry,const char *fpath,int is_req);

extern void ch_smon_session_entry_write(ch_smon_session_entry_t *smon_entry,void *data,size_t dlen,int is_req);

extern void ch_smon_session_entry_fin(ch_smon_session_entry_t *smon_entry);


#endif /*CH_SMON_SESSION_ENTRY_H*/
