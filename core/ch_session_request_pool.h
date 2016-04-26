/*
 * =====================================================================================
 *
 *       Filename:  ch_session_request_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月05日 11时21分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SESSION_REQUEST_POOL_H
#define CH_SESSION_REQUEST_POOL_H


typedef struct ch_session_request_pool_t ch_session_request_pool_t;
typedef struct ch_session_request_t ch_session_request_t;

#include <apr_pools.h>
#include "ch_context.h"
#include "ch_list.h"
#include "ch_assemble_task.h"
#include "ch_packet.h"
#include "ch_hash_pool.h"

enum {

    THREE_WAY_INIT = 0,
    THREE_WAY_SYN,
    THREE_WAY_SYN_ACK,
    THREE_WAY_ACK_ACK
};

struct ch_session_request_pool_t {

    ch_hash_pool_t *hash_pool;
    ch_context_t *context;
    apr_pool_t *mp;

    size_t n_requests;

};

struct ch_session_request_t {

    ch_hash_entry_t entry;
    uint32_t req_sn_init;
    uint32_t res_sn_init;

    uint32_t req_ip;
    uint16_t req_port;
    
    uint32_t res_ip;
    uint16_t res_port;

    uint8_t three_way_state;
};

extern ch_session_request_pool_t * ch_session_request_pool_create(ch_context_t *context);

extern void ch_session_request_pool_destroy(ch_session_request_pool_t *req_pool);

extern ch_session_request_t* ch_session_request_create(ch_session_request_pool_t *req_pool,ch_packet_info_t *pinfo);


extern void ch_session_request_free(ch_session_request_pool_t *req_pool,ch_session_request_t *ses_req);


extern ch_session_request_t* ch_session_request_find(ch_session_request_pool_t *req_pool,ch_packet_info_t *pinfo);


extern ch_assemble_task_t * ch_assemble_task_get_from_request_pool(ch_session_request_pool_t *req_pool);

#endif /*CH_SESSION_REQUEST_POOL_H*/
