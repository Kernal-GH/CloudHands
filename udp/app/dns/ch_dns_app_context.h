/*
 * =====================================================================================
 *
 *       Filename:  ch_dns_app_context.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/30/2019 04:29:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_DNS_APP_CONTEXT_H
#define CH_DNS_APP_CONTEXT_H

typedef struct ch_dns_app_context_t ch_dns_app_context_t;
#include "ch_tables.h"
#include "ch_redis_ip_wblist.h"

struct ch_dns_app_context_t {

    ch_pool_t *mp;

    const char *wblist_fpath;
    const char *redis_cfname;

    ch_redis_ip_wblist_t *ip_wblist;

    ch_array_header_t *wblist_domains;

};

extern ch_dns_app_context_t *ch_dns_app_context_create(const char *cfname);

extern int ch_dns_app_context_wblist_domain_is_accept(ch_dns_app_context_t *dns_context,const char *domain);

#endif /*CH_DNS_APP_CONTEXT_H */

