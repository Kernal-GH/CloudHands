/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session_endpoint.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月18日 11时13分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_tcp_session_endpoint.h"
#include "ch_util.h"

void ch_tcp_session_endpoint_init(ch_tcp_session_endpoint_t *ep,uint32_t ip,uint16_t port,uint32_t init_seq,ch_memory_t *mm){

    ch_assemble_fragment_init(&ep->as_frag,mm);

    ep->last_offset = 0;
	ep->last_ack = 0;
    ep->init_seq = init_seq;
    ep->ip = ip;
    ep->port = port;
    
    ep->is_ipv6 = 0;
    ep->priv_data = NULL;
    ep->time = ch_get_current_timems();
}

void ch_tcp_session_endpoint_init_ipv6(ch_tcp_session_endpoint_t *ep,uint8_t *addr,uint16_t port,uint32_t init_seq,ch_memory_t *mm){

    ch_assemble_fragment_init(&ep->as_frag,mm);

    ep->last_offset = 0;
	ep->last_ack = 0;
    ep->init_seq = init_seq;
    ep->ip = 0;
    ep->port = port;
    ep->priv_data = NULL;
    memcpy(ep->addr,addr,16);
    ep->is_ipv6 = 1;
    ep->time = ch_get_current_timems();
}

void ch_tcp_session_endpoint_fin(ch_tcp_session_endpoint_t *ep){

    ch_assemble_fragment_fin(&ep->as_frag);
}

int ch_tcp_session_endpoint_do(ch_tcp_session_endpoint_t *ep,void *data,size_t dlen,size_t offset){


    return ch_assemble_fragment_push(&ep->as_frag,data,dlen,offset);
}



