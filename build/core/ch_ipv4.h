/*
 * =====================================================================================
 *
 *       Filename:  ch_ipv4.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月27日 11时11分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_IPV4_H
#define CH_IPV4_H

typedef struct ch_ipv4_t ch_ipv4_t;
typedef struct ch_ipv4_header_t ch_ipv4_header_t;

#include <apr_pools.h>
#include "ch_protocol_type.h"
#include "ch_ethertype.h"
#include "ch_context.h"


struct ch_ipv4_t {
    ch_context_t *context;
    ch_protocol_type_t* protos[CH_IPPROTO_MAX];

};

extern ch_ipv4_t * ch_ipv4_create(ch_context_t *context,ch_ethertype_t *ethertype);

static inline void ch_ipv4_proto_register(ch_ipv4_t *ipv4,ch_protocol_type_t *prototype){
    
    if(prototype->proto<CH_IPPROTO_IP||prototype->proto>=CH_IPPROTO_MAX)
        return;

    ipv4->protos[prototype->proto] = prototype;
}

static inline ch_protocol_type_t * ch_ipv4_proto_get(ch_ipv4_t *ipv4,uint8_t type){
    
    if(type<CH_IPPROTO_IP||type>=CH_IPPROTO_MAX)
        return NULL;

    return ipv4->protos[type];
}

#endif /*CH_IPV4_H*/

