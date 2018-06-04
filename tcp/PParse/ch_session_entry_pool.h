/*
 *
 *      Filename: ch_session_entry_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 17:19:06
 * Last Modified: 2018-05-14 17:19:06
 */

#ifndef CH_SESSION_ENTRY_POOL_H
#define CH_SESSION_ENTRY_POOL_H

typedef struct ch_session_entry_pool_t ch_session_entry_pool_t;

#include "ch_ptable.h"
#include "ch_pp_pool.h"
#include "ch_tcp_record.h"
#include "ch_session_entry.h"

struct ch_session_entry_pool_t {

	ch_pool_t *mp;
	ch_pp_context_t *pcontext;

	ch_ptable_t *sentry_tbl;

};

extern ch_session_entry_pool_t * ch_session_entry_pool_create(ch_pp_context_t *pcontext,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data);

extern ch_session_entry_t *
ch_session_entry_pool_entry_create(ch_session_entry_pool_t *sentry_pool,ch_pp_parser_t *pparser,ch_tcp_record_t *tcp_record);


#define ch_session_entry_pool_entry_find(sentry_pool,tcp_record) (ch_session_entry_t*)ch_ptable_entry_find(sentry_pool->sentry_tbl,(void*)tcp_record)


#define ch_session_entry_pool_entry_free(sentry_pool,sentry) ch_ptable_entry_free(sentry_pool->sentry_tbl,(ch_ptable_entry_t*)sentry)

#define ch_session_entry_pool_timeout_free(sentry_pool) ch_ptable_entries_timeout_free(sentry_pool->sentry_tbl,NULL) 

#define ch_session_entry_pool_tbl_dump(sentry_pool) ch_ptable_dump(sentry_pool->sentry_tbl,stdout)

#endif /*CH_SESSION_ENTRY_POOL_H*/
