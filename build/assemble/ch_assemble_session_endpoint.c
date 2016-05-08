/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_endpoint.c
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
#include "ch_assemble_session_endpoint.h"

void ch_assemble_session_endpoint_init(ch_assemble_session_endpoint_t *ep,uint32_t ip,uint16_t port,uint32_t init_seq){

    ch_assemble_fragment_init(&ep->as_frag);

    ep->last_offset = 0;
    ep->init_seq = init_seq;
    ep->ip = ip;
    ep->port = port;
    ep->priv_data = NULL;
}

void ch_assemble_session_endpoint_fin(ch_assemble_session_endpoint_t *ep){

    ch_assemble_fragment_fin(&ep->as_frag);
}

int ch_assemble_session_endpoint_do(ch_assemble_session_endpoint_t *ep,void *data,size_t dlen,size_t offset){


    return ch_assemble_fragment_push(&ep->as_frag,data,dlen,offset);
}



