/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_context_tcp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月29日 14时40分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_ASSEMBLE_CONTEXT_TCP_H
#define CH_ASSEMBLE_CONTEXT_TCP_H

typedef struct ch_assemble_context_tcp_t ch_assemble_context_tcp_t;

#include "ch_context.h"

struct ch_assemble_context_tcp_t {

    ch_context_t *context;
    const char *mmap_file_dir;
    uint16_t assemble_tasks_n;
    
    uint32_t tcp_session_requests_limits;
    uint32_t tcp_assemble_sessions_limits;
    
    uint32_t tcp_assemble_sessions_tbl_size;
    uint32_t tcp_session_requests_tbl_size;

    uint32_t tcp_assemble_sessions_tbl_caches_limits;
    uint32_t tcp_session_requests_tbl_caches_limits;

    uint64_t tcp_session_request_timeout;
    uint64_t tcp_assemble_session_timeout;
    
    uint64_t mmap_file_size;
    uint64_t mmap_file_entry_size;

};

extern int ch_assemble_context_tcp_init(ch_context_t *context,ch_assemble_context_tcp_t *tcp_context);

#endif /*CH_ASSEMBLE_CONTEXT_TCP_H*/
