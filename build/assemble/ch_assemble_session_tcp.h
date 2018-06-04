/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_tcp_tcp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月30日 14时34分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ASSEMBLE_SESSION_TCP_H
#define CH_ASSEMBLE_SESSION_TCP_H

typedef struct ch_assemble_session_tcp_t ch_assemble_session_tcp_t;

#include <apr_pools.h>
#include "ch_ptable.h"
#include "ch_assemble_session_endpoint.h"
#include "ch_tcp_session_request_pool.h"
#include "ch_util.h"
#include "ch_app_context.h"
#include "ch_assemble.h"

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

struct  ch_assemble_session_tcp_t {

    ch_ptable_entry_t entry;

    ch_assemble_session_endpoint_t endpoint_req;
    
    ch_assemble_session_endpoint_t endpoint_res;

    ch_app_t *app;
    
    ch_assemble_t *as;
    ch_assemble_session_endpoint_t *cur_ep;

    uint64_t session_id;
    uint32_t fin1_seq;
    uint32_t fin2_seq;

    unsigned state:2;

    unsigned need_end:1;

    unsigned four_way_state:5;
};

#define ch_assemble_session_tcp_srcport_get(ass) ((ass)->endpoint_req.port)
#define ch_assemble_session_tcp_dstport_get(ass) ((ass)->endpoint_res.port)
#define ch_assemble_session_tcp_srcip_get(ass)   ((ass)->endpoint_req.ip)
#define ch_assemble_session_tcp_dstip_get(ass)   ((ass)->endpoint_res.ip)

#define ch_assemble_session_tcp_state_set(ass,st) ((ass)->state = st)

#define ch_tcp_session_state_is_fin(ass) ((ass)->state == SESSION_STATE_FIN)

#define ch_tcp_session_state_is_rst(ass) ((ass)->state == SESSION_STATE_RST)

extern int ch_assemble_session_tcp_init(ch_assemble_t *as,ch_assemble_session_tcp_t *ass,ch_tcp_session_request_t *sreq,ch_app_t *app); 

extern void ch_assemble_session_tcp_fin(ch_assemble_session_tcp_t *ass);

extern ch_assemble_session_endpoint_t * ch_assemble_session_tcp_endpoint_get(ch_assemble_session_tcp_t *ass,ch_assemble_packet_t *as_pkt);

extern ch_assemble_session_endpoint_t * ch_assemble_session_tcp_pair_endpoint_get(ch_assemble_session_tcp_t *ass,ch_assemble_packet_t *as_pkt);

#endif /*CH_ASSEMBLE_SESSION_TCP_H*/
