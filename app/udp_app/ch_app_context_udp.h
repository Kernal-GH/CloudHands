/*
 *
 *      Filename: ch_app_context_udp.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-14 18:43:37
 * Last Modified: 2016-10-14 18:43:37
 */

#ifndef CH_APP_CONTEXT_UDP_H
#define CH_APP_CONTEXT_UDP_H

typedef struct ch_app_context_udp_t ch_app_context_udp_t;

#include "ch_app_context.h"

struct ch_app_context_udp_t {

    ch_app_context_t app_context;

    int dns_is_on;

};

extern ch_app_context_t * ch_app_context_udp_create(ch_context_t *context);

#endif /* CH_APP_CONTEXT_UDP_H */
