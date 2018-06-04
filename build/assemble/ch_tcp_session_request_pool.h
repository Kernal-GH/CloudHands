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

#include <apr_pools.h>
#include "ch_list.h"
#include "ch_ptable.h"
#include "ch_assemble_context_tcp.h"
#include "ch_assemble_packet.h"

enum {

    THREE_WAY_INIT = 0,
    THREE_WAY_SYN,
    THREE_WAY_SYN_ACK,
    THREE_WAY_ACK_ACK
};

struct ch_tcp_session_request_pool_t {

    apr_pool_t *mp;
    ch_ptable_t *as_tbl;
    ch_assemble_context_tcp_t *tcp_context;

    size_t n_requests;
};

struct ch_tcp_session_request_t {

    ch_ptable_entry_t entry;
    uint32_t req_sn_init;
    uint32_t res_sn_init;

    uint32_t req_ip;
    uint16_t req_port;
    
    uint32_t res_ip;
    uint16_t res_port;

    uint8_t three_way_state;
};

extern ch_tcp_session_request_pool_t * ch_tcp_session_request_pool_create(ch_assemble_context_tcp_t *tcp_context);

extern void ch_tcp_session_request_pool_destroy(ch_tcp_session_request_pool_t *req_pool);

extern ch_tcp_session_request_t* ch_tcp_session_request_create(ch_tcp_session_request_pool_t *req_pool,ch_assemble_packet_t *as_pkt);


extern void ch_tcp_session_request_free(ch_tcp_session_request_pool_t *req_pool,ch_tcp_session_request_t *ses_req);


extern ch_tcp_session_request_t* ch_tcp_session_request_find(ch_tcp_session_request_pool_t *req_pool,ch_assemble_packet_t *as_pkt);

#endif /*CH_TCP_SESSION_REQUEST_POOL_H*/
