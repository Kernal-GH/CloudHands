/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session_tcp.h
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

typedef struct ch_tcp_session_t ch_tcp_session_t;

#include "ch_mpool.h"
#include "ch_ptable.h"
#include "ch_tcp_session_endpoint.h"
#include "ch_tcp_session_request_pool.h"
#include "ch_util.h"
#include "ch_memory.h"
#include "ch_packet_tcp.h"
#include "ch_tcp_app_pool.h"
#include "ch_tcp_session_pool.h"

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

struct  ch_tcp_session_t {

    ch_ptable_entry_t entry;

    ch_tcp_session_endpoint_t endpoint_req;
    
    ch_tcp_session_endpoint_t endpoint_res;

	ch_tcp_session_pool_t *ts_pool;

    ch_tcp_app_t *app;
    void *sentry;

    ch_tcp_session_endpoint_t *cur_ep;

    uint64_t session_id;
	uint32_t fin1_seq;
    uint32_t fin2_seq;

    unsigned state:2;

    unsigned need_end:1;

    unsigned four_way_state:5;
};

#define ch_tcp_session_srcport_get(tcp_session) ((tcp_session)->endpoint_req.port)
#define ch_tcp_session_dstport_get(tcp_session) ((tcp_session)->endpoint_res.port)
#define ch_tcp_session_srcip_get(tcp_session)   ((tcp_session)->endpoint_req.ip)
#define ch_tcp_session_dstip_get(tcp_session)   ((tcp_session)->endpoint_res.ip)

#define ch_tcp_session_reqtime_get(tcp_session)   ((tcp_session)->endpoint_req.time)
#define ch_tcp_session_restime_get(tcp_session)   ((tcp_session)->endpoint_res.time)

#define ch_tcp_session_state_set(tcp_session,st) ((tcp_session)->state = st)

#define ch_tcp_session_state_is_fin(tcp_session) ((tcp_session)->state == SESSION_STATE_FIN)

#define ch_tcp_session_state_is_rst(tcp_session) ((tcp_session)->state == SESSION_STATE_RST)

#define ch_tcp_session_is_ipv6(tcp_session) ((tcp_session)->endpoint_req.is_ipv6)
#define ch_tcp_session_srcaddr_get(tcp_session) ((tcp_session)->endpoint_req.addr)
#define ch_tcp_session_dstaddr_get(tcp_session) ((tcp_session)->endpoint_res.addr)

extern int ch_tcp_session_init(ch_tcp_session_t *tcp_session,ch_tcp_session_request_t *sreq,ch_tcp_app_t *app,
        void *sentry,
        ch_memory_t *mm); 

extern void ch_tcp_session_fin(ch_tcp_session_t *tcp_session);

extern ch_tcp_session_endpoint_t * ch_tcp_session_endpoint_get(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt);

extern ch_tcp_session_endpoint_t * ch_tcp_session_pair_endpoint_get(ch_tcp_session_t *tcp_session,ch_packet_tcp_t *tcp_pkt);

static inline void ch_tcp_session_dump(ch_tcp_session_t *session,FILE *fp){

	fprintf(fp,"Dump TCP Session:%lu\n",(unsigned long)session->session_id);
	
	fprintf(fp,"Dump TCP Request:\n");
	ch_tcp_session_endpoint_dump(&session->endpoint_req,fp);

	fprintf(fp,"Dump TCP Response:\n");
	ch_tcp_session_endpoint_dump(&session->endpoint_res,fp);

}
#endif /*CH_ASSEMBLE_SESSION_TCP_H*/
