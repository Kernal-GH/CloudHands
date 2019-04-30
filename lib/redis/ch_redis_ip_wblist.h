/*
 * =====================================================================================
 *
 *       Filename:  ch_redis_ip_wblist.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2019 01:37:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_REDIS_IP_WBLIST_H
#define CH_REDIS_IP_WBLIST_H

typedef struct ch_redis_ip_wblist_t ch_redis_ip_wblist_t;

#include "ch_redis_ipbitmap.h"
#include "ch_mpool.h"

struct ch_redis_ip_wblist_t {
    
    ch_pool_t *mp;
    const char *host;
    const char *passwd;
    const char *iptab_name;
    uint16_t port;

    int is_on;
    int is_blacklist;

    ch_redis_t *redis;
};

extern ch_redis_ip_wblist_t * ch_redis_ip_wblist_create(ch_pool_t *mp,const char *cfname);

static inline int ch_redis_ip_wblist_accept(ch_redis_ip_wblist_t *ip_wblist,uint32_t srcIP,uint32_t dstIP){


    if(!ip_wblist->is_on||ip_wblist->redis == NULL)
        return 1;


    int is_in = ch_redis_ipbitmap_srcORdstIP_is_in(ip_wblist->redis,ip_wblist->iptab_name,srcIP,dstIP);


    return ip_wblist->is_blacklist?(!is_in):(is_in);
}

static inline int ch_redis_ip_wblist_add(ch_redis_ip_wblist_t *ip_wblist,uint32_t ip){

    if(ip_wblist->redis){

        return ch_redis_ipbitmap_set(ip_wblist->redis,ip_wblist->iptab_name,ip);
    }

    return 0;
}


#endif /* CH_REDIS_IP_WBLIST_H */

