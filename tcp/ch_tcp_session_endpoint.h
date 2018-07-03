/*
 * =====================================================================================
 *
 *       Filename:  ch_tcp_session_endpoint.h
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
#ifndef CH_TCP_SESSION_ENDPOINT_H
#define CH_TCP_SESSION_ENDPOINT_H

typedef struct ch_tcp_session_endpoint_t ch_tcp_session_endpoint_t;

#include "ch_mpool.h"
#include "ch_assemble_fragment.h"
#include "ch_memory.h"

struct ch_tcp_session_endpoint_t {

    ch_assemble_fragment_t as_frag;

    uint64_t time;
    /*The offset waiting to need!!*/
    uint32_t last_offset;

	/* The last ack seq */
	uint32_t last_ack;

    uint32_t init_seq;

    uint32_t ip;
    
    uint16_t port;


	uint64_t packets;
	uint64_t bytes;

    void *priv_data;

};

#define ch_tcp_session_endpoint_update(ep,dlen) do { \
	(ep)->packets+=1;								 \
	(ep)->bytes+=(dlen);							 \
}while(0) 

static inline uint32_t ch_tcp_session_endpoint_offset_get(ch_tcp_session_endpoint_t *ep,uint32_t seq){

    return seq-ep->init_seq;
}

/*init a session endpoint instance 
 * @ip,the ip address of this endpoint
 * @port,the port of this endpoint
 * @init_seq,the init sequence
 * 
 */
extern void ch_tcp_session_endpoint_init(ch_tcp_session_endpoint_t *ep,uint32_t ip,uint16_t port,uint32_t init_seq,ch_memory_t *mm);


/*Clean the instance of assemble session endpoint
 * @endpoint,the instance of assemble session endpoint*/
extern void ch_tcp_session_endpoint_fin(ch_tcp_session_endpoint_t *ep);



/*Assemble data fragment for this endpoint
 * @data,the data fragment payload
 * @d_len,the length of data fragment payload
 * @offset,the offset to start 
 *return: if ok,return 0,otherwise return -1*/
extern int ch_tcp_session_endpoint_do(ch_tcp_session_endpoint_t *ep,void *data,size_t dlen,size_t offset);

#endif /*CH_TCP_SESSION_ENDPOINT_H*/

