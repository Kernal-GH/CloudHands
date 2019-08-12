/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session_request_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月29日 11时21分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_TCP_SESSION_REQUEST_POOL_H
#define CH_TCP_SESSION_REQUEST_POOL_H


typedef struct ch_tcp_session_request_pool_t ch_tcp_session_request_pool_t;
typedef struct ch_tcp_session_request_t ch_tcp_session_request_t;

#include "ch_mpool.h"
#include "ch_list.h"
#include "ch_ptable.h"
#include "ch_tcp_context.h"
#include "ch_packet_tcp.h"

enum {

    THREE_WAY_INIT = 0,
    THREE_WAY_SYN,
    THREE_WAY_SYN_ACK,
    THREE_WAY_ACK_ACK
};

struct ch_tcp_session_request_pool_t {

    ch_pool_t *mp;
    ch_ptable_t *tcp_session_request_tbl;

    ch_tcp_context_t *tcp_context;

    size_t n_requests;
};

struct ch_tcp_session_request_t {

    ch_ptable_entry_t entry;
    uint32_t req_sn_init;
    uint32_t res_sn_init;

    uint8_t is_ipv6;

    uint32_t req_ip;
    uint8_t  src_addr[16];
    uint16_t req_port;
    
    uint32_t res_ip;
    uint8_t  dst_addr[16];
    uint16_t res_port;

    uint8_t three_way_state;
};

extern ch_tcp_session_request_pool_t * ch_tcp_session_request_pool_create(ch_tcp_context_t *tcp_context,
	size_t priv_data_size,
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data),
	void *priv_data);

extern void ch_tcp_session_request_pool_destroy(ch_tcp_session_request_pool_t *req_pool);

extern ch_tcp_session_request_t* ch_tcp_session_request_create(ch_tcp_session_request_pool_t *req_pool,
	ch_packet_tcp_t *tcp_pkt);

extern void ch_tcp_session_request_free(ch_tcp_session_request_pool_t *req_pool,ch_tcp_session_request_t *ses_req);


extern ch_tcp_session_request_t* ch_tcp_session_request_find(ch_tcp_session_request_pool_t *req_pool,ch_packet_tcp_t *tcp_pkt);

#endif /*CH_TCP_SESSION_REQUEST_POOL_H*/
