/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月26日 18时15分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_TCP_SESSION_H
#define CH_TCP_SESSION_H

typedef struct ch_tcp_session_t ch_tcp_session_t;

#include "ch_context.h"
#include "ch_session_request_pool.h"

struct ch_tcp_session_t {

    ch_context_t *context;
    ch_session_request_pool_t *req_pool;
};

extern ch_tcp_session_t * ch_tcp_session_create(ch_context_t *context);

extern int ch_tcp_session_packet_process(ch_tcp_session_t *tsession,ch_packet_info_t *pinfo);

extern void ch_tcp_session_destroy(ch_tcp_session_t *tsession);
#endif /*CH_TCP_SESSION_H*/
