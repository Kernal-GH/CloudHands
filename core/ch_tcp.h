/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月29日 14时32分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_TCP_H
#define CH_TCP_H

typedef struct ch_tcp_t ch_tcp_t;

#include "ch_context.h"
#include "ch_ipv4.h"
#include "ch_session_request_pool.h"


struct ch_tcp_t {
    
    ch_context_t *context;
    ch_session_request_pool_t *req_pool;
};

extern ch_tcp_t * ch_tcp_create(ch_context_t *context,ch_ipv4_t *ipv4);

extern void ch_tcp_destroy(ch_tcp_t *tcp);

#endif /*CH_TCP_H*/

