/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_pool_tcp.h
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

#ifndef CH_ASSEMBLE_SESSION_POOL_TCP_H
#define CH_ASSEMBLE_SESSION_POOL_TCP_H

typedef struct ch_assemble_session_pool_tcp_t ch_assemble_session_pool_tcp_t;
#include <apr_pools.h>
#include "ch_ptable.h"
#include "ch_assemble_session_tcp.h"
#include "ch_assemble.h"

struct ch_assemble_session_pool_tcp_t {

    rte_spinlock_t spin_lock;
    
	apr_pool_t *mp;
    ch_assemble_context_tcp_t *tcp_context;

    ch_ptable_t *as_tbl;

    ch_assemble_t *as;

    size_t n_sessions;
};

#define ch_assemble_session_pool_lock(aspt) rte_spinlock_lock(&(aspt)->spin_lock)
#define ch_assemble_session_pool_unlock(aspt) rte_spinlock_unlock(&(aspt)->spin_lock)

extern ch_assemble_session_pool_tcp_t * ch_assemble_session_pool_tcp_create(ch_assemble_t *as,ch_assemble_context_tcp_t *tcp_context);

extern void ch_assemble_session_pool_tcp_destroy(ch_assemble_session_pool_tcp_t *asspt);

extern ch_assemble_session_tcp_t * 
ch_assemble_session_pool_tcp_entry_create(ch_assemble_session_pool_tcp_t *asspt,ch_tcp_session_request_t *sreq,ch_app_t *app,
        ch_assemble_packet_t *as_pkt);


extern ch_assemble_session_tcp_t * ch_assemble_session_pool_tcp_entry_find(ch_assemble_session_pool_tcp_t *asspt,ch_assemble_packet_t *as_pkt);


extern void ch_assemble_session_pool_tcp_entry_free(ch_assemble_session_pool_tcp_t *asspt,ch_assemble_session_tcp_t *ass);

#endif /*CH_ASSEMBLE_SESSION_POOL_TCP_H*/

