/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_endpoint.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月01日 10时51分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_ASSEMBLE_SESSION_ENDPOINT_H
#define CH_ASSEMBLE_SESSION_ENDPOINT_H

typedef struct ch_assemble_session_endpoint_t ch_assemble_session_endpoint_t;

#include <apr_pools.h>
#include "ch_assemble_fragment.h"

struct ch_assemble_session_endpoint_t {

    ch_assemble_fragment_t as_frag;
    
    /*The offset waiting to need!!*/
    uint32_t last_offset;

    uint32_t init_seq;

    uint32_t ip;
    
    uint16_t port;

    
    void *priv_data;

};


/*init a session endpoint instance 
 * @ip,the ip address of this endpoint
 * @port,the port of this endpoint
 * @init_seq,the init sequence
 * 
 */
extern void ch_assemble_session_endpoint_init(ch_assemble_session_endpoint_t *ep,uint32_t ip,uint16_t port,uint32_t init_seq);


/*Clean the instance of assemble session endpoint
 * @endpoint,the instance of assemble session endpoint*/
extern void ch_assemble_session_endpoint_fin(ch_assemble_session_endpoint_t *ep);



/*Assemble data fragment for this endpoint
 * @data,the data fragment payload
 * @d_len,the length of data fragment payload
 * @offset,the offset to start 
 *return: if ok,return 0,otherwise return -1*/
extern int ch_assemble_session_endpoint_do(ch_assemble_session_endpoint_t *ep,void *data,size_t dlen,size_t offset);

#endif /*CH_ASSEMBLE_SESSION_ENDPOINT_H*/

