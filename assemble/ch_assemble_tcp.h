/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_tcp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月29日 15时52分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ASSEMBLE_TCP_H
#define CH_ASSEMBLE_TCP_H

typedef struct ch_assemble_tcp_t ch_assemble_tcp_t;

#include "ch_assemble.h"
#include "ch_assemble_session_pool_tcp.h"
#include "ch_assemble_pool_tcp.h"

struct ch_assemble_tcp_t {

    ch_assemble_t as;

    ch_assemble_context_tcp_t *tcp_context;

    ch_assemble_session_pool_tcp_t *asspt;
    ch_assemble_pool_tcp_t *aspt;
};

extern ch_assemble_t * ch_assemble_tcp_create(ch_assemble_pool_tcp_t *aspt,int assemble_id);

extern void ch_assemble_tcp_destroy(ch_assemble_tcp_t *tas);

#endif /*CH_ASSEMBLE_TCP_H*/

