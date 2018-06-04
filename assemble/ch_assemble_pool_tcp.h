/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_pool_tcp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月28日 16时21分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ASSEMBLE_POOL_TCP_H
#define CH_ASSEMBLE_POOL_TCP_H

typedef struct ch_assemble_pool_tcp_t ch_assemble_pool_tcp_t;

#include "ch_tcp_session_request_pool.h"
#include "ch_assemble_pool.h"
#include "ch_assemble_context_tcp.h"
#include "ch_app_context_tcp.h"

struct ch_assemble_pool_tcp_t {

    ch_assemble_pool_t asp;
    ch_assemble_context_tcp_t tcp_context;
    ch_app_context_t *app_tcp_context;

    ch_tcp_session_request_pool_t *tsr_pool;
};

extern ch_assemble_pool_t * ch_assemble_pool_tcp_create(ch_context_t *context);


extern void ch_assemble_pool_tcp_destroy(ch_assemble_pool_t *asp);

#endif /*CH_ASSEMBLE_POOL_TCP_H*/
