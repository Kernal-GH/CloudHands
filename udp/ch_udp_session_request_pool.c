/*
 *
 *      Filename: ch_udp_session_request_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-12 17:23:36
 * Last Modified: 2018-09-13 17:21:08
 */

#include "ch_udp_session_request_pool.h"
#include "ch_log.h"
#include "ch_udp_app_pool.h"

static int _udp_session_request_entry_equal(ch_plist_entry_t *entry,void *key,void *priv_data ch_unused){

	ch_packet_udp_t *udp_pkt = (ch_packet_udp_t*)key;

    ch_udp_session_request_t *req_session = (ch_udp_session_request_t*)entry;

    return ch_udp_app_session_request_equal(req_session,req_session->app_session,udp_pkt);

}

ch_udp_session_request_pool_t *ch_udp_session_request_pool_create(ch_udp_work_t *udp_work,
	void (*entry_timeout_cb)(ch_plist_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data){

    ch_udp_session_request_pool_t *udp_pool;
	ch_udp_context_t *udp_context = udp_work->udp_context;

    ch_pool_t *mp = udp_work->mp;

    udp_pool = (ch_udp_session_request_pool_t*)ch_palloc(mp,sizeof(*udp_pool));

    udp_pool->mp = mp;
	udp_pool->work = udp_work;

    udp_pool->plist = ch_plist_create(mp,
		    udp_context->udp_session_request_pool_type, 
		    sizeof(ch_udp_session_request_t),
			0,
            udp_context->udp_session_request_limits,
            udp_context->udp_session_request_timeout,
            priv_data,
            _udp_session_request_entry_equal,
            NULL,
			entry_timeout_cb);

    if(udp_pool->plist == NULL){
        ch_log(CH_LOG_ERR,"Cannot create  udp session request plist  failed for udp session request pool!");
        return NULL;
    }
    

    return udp_pool;

}
