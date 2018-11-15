/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月14日 11时23分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_TCP_SESSION_POOL_H
#define CH_TCP_SESSION_POOL_H

typedef struct ch_tcp_session_pool_t ch_tcp_session_pool_t;

#include "ch_mpool.h"
#include "ch_ptable.h"
#include "ch_tcp_session.h"
#include "ch_tcp_context.h"
#include "ch_memory.h"
#include "ch_packet_tcp.h"
#include "ch_tcp_session_handler.h"

struct ch_tcp_session_pool_t {

	ch_pool_t *mp;
    ch_tcp_context_t *tcp_context;

    ch_tcp_session_handler_t *shandler;

    ch_ptable_t *tcp_session_tbl;

	ch_memory_t *mm;

};

extern ch_tcp_session_pool_t * ch_tcp_session_pool_create(ch_tcp_context_t *tcp_context,
	size_t priv_data_size,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data);

extern void ch_tcp_session_pool_destroy(ch_tcp_session_pool_t *tspt);

extern ch_tcp_session_t * 
ch_tcp_session_pool_entry_create(ch_tcp_session_pool_t *tspt,ch_tcp_session_request_t *sreq,ch_tcp_app_t *app,void *sentry,
        ch_packet_tcp_t *tcp_pkt);


extern ch_tcp_session_t * ch_tcp_session_pool_entry_find(ch_tcp_session_pool_t *tspt,ch_packet_tcp_t *tcp_pkt);


#define ch_tcp_session_pool_entry_free(tspt,tcp_session) ch_ptable_entry_free(tspt->tcp_session_tbl,(ch_ptable_entry_t*)tcp_session)

#endif /*CH_TCP_SESSION_POOL_H*/

