/*
 *
 *      Filename: ch_udp_session_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-10 15:35:57
 * Last Modified: 2018-05-10 15:35:57
 */

#ifndef CH_UDP_SESSION_POOL_H
#define CH_UDP_SESSION_POOL_H

typedef struct ch_udp_session_pool_t ch_udp_session_pool_t;

#include "ch_udp_work.h"
#include "ch_mpool.h"
#include "ch_ptable.h"
#include "ch_udp_session.h"
#include "ch_packet_udp.h"

struct ch_udp_session_pool_t {

	ch_pool_t *mp;
	
	ch_udp_work_t *work;

	ch_ptable_t *udp_session_tbl;

};

extern ch_udp_session_pool_t *ch_udp_session_pool_create(ch_udp_work_t *work,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data);

extern ch_udp_session_t *
ch_udp_session_pool_entry_create(ch_udp_session_pool_t *udp_pool,ch_packet_udp_t *udp_pkt,ch_udp_app_session_t *app_session,
	uint64_t session_id);


#define ch_udp_session_pool_destroy(udp_pool) ch_pool_destroy(udp_pool->mp)


#define ch_udp_session_pool_entry_find(udp_pool,udp_pkt) (ch_udp_session_t*)ch_ptable_entry_find(udp_pool->udp_session_tbl,(void*)udp_pkt)


#define ch_udp_session_pool_entry_free(udp_pool,udp_session) ch_ptable_entry_free(udp_pool->udp_session_tbl,(ch_ptable_entry_t*)udp_session)


#endif /*CH_UDP_SESSION_POOL_H*/
