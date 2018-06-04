/*
 *
 *      Filename: ch_session_entry_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 17:36:08
 * Last Modified: 2018-05-17 19:02:17
 */

#include "ch_session_entry_pool.h"
#include "ch_session_entry.h"
#include "ch_pp_context.h"
#include "ch_log.h"

static size_t _session_entry_hash(void *key,void *priv_data ch_unused){

	ch_tcp_record_t *tcp_record = (ch_tcp_record_t*)key;

	return tcp_record->session_id;
}

static int _session_entry_equal(ch_ptable_entry_t *entry,void *key,void *priv_data ch_unused){

	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;
	ch_tcp_record_t *tcp_record = (ch_tcp_record_t*)key;

	return sentry->session_id == tcp_record->session_id;
}

ch_session_entry_t *
ch_session_entry_pool_entry_create(ch_session_entry_pool_t *sentry_pool,ch_pp_parser_t *pparser,ch_tcp_record_t *tcp_record){


	ch_session_entry_t *sentry = (ch_session_entry_t*)ch_ptable_entry_create(sentry_pool->sentry_tbl,(void*)tcp_record);

	if(sentry)
	{

		if(ch_session_entry_init(sentry,pparser,tcp_record))
			return NULL;

	}

	return sentry;

}

ch_session_entry_pool_t * ch_session_entry_pool_create(ch_pp_context_t *pcontext,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data){

	ch_session_entry_pool_t *sentry_pool = NULL;

	sentry_pool = (ch_session_entry_pool_t*)ch_pcalloc(pcontext->mp,sizeof(*sentry_pool));

	sentry_pool->mp = pcontext->mp;
	sentry_pool->pcontext = pcontext;

    sentry_pool->sentry_tbl = ch_ptable_create(sentry_pool->mp,
		    pcontext->tbl_pool_type, 
		    sizeof(ch_session_entry_t),
			0,
            pcontext->tbl_session_limits,
            pcontext->tbl_session_tbl_size,
            pcontext->tbl_session_timeout,
            pcontext->tbl_session_cache_limits,
            priv_data,
            _session_entry_hash,
            _session_entry_equal,
            NULL,
			entry_timeout_cb);

	if(sentry_pool->sentry_tbl == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create session entry table !");
		return NULL;
	}

	return sentry_pool;
}


