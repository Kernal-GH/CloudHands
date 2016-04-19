/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月26日 11时46分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ASSEMBLE_SESSION_H
#define CH_ASSEMBLE_SESSION_H

typedef struct ch_assemble_session_t ch_assemble_session_t;

#include <apr_pools.h>
#include "ch_hash_pool.h"
#include "ch_assemble_session_endpoint.h"
#include "ch_session_request_pool.h"
#include "ch_util.h"
#include "ch_app_context.h"
#include "ch_assemble_task.h"

enum {
    SESSION_STATE_INIT=0,
    SESSION_STATE_DATA,
    SESSION_STATE_FIN,
    SESSION_STATE_RST
};

enum {
    FOUR_WAY_INIT=0,
    FOUR_WAY_FIN1,
    FOUR_WAY_FIN1_ACK,
    FOUR_WAY_FIN2,
    FOUR_WAY_FIN2_ACK
};

struct  ch_assemble_session_t {

    ch_hash_entry_t entry;

    ch_assemble_session_endpoint_t endpoint_req;
    
    ch_assemble_session_endpoint_t endpoint_res;

    ch_app_t *app;
    ch_assemble_task_t *astask;
    ch_assemble_session_endpoint_t *cur_ep;

    uint32_t fin1_seq;
    uint32_t fin2_seq;

    unsigned state:2;

    unsigned need_end:1;

    unsigned four_way_state:5;
};

#define ch_assemble_session_state_set(ass,st) ((ass)->state = st)

#define session_state_is_fin(ass) ((ass)->state == SESSION_STATE_FIN)

#define session_state_is_rst(ass) ((ass)->state == SESSION_STATE_RST)

extern int ch_assemble_session_init(ch_assemble_task_t *astask,ch_assemble_session_t *ass,ch_session_request_t *sreq); 

extern void ch_assemble_session_fin(ch_assemble_session_t *ass);

extern ch_assemble_session_endpoint_t * ch_assemble_session_endpoint_get(ch_assemble_session_t *ass,ch_four_tuple_t *tuple);

#endif /*CH_ASSEMBLE_SESSION_H*/
