/*
 *
 *      Filename: ch_assemble_pool_udp.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-14 15:11:57
 * Last Modified: 2016-10-14 15:11:57
 */

#ifndef CH_ASSEMBLE_POOL_UDP_H
#define CH_ASSEMBLE_POOL_UDP_H

typedef struct ch_assemble_pool_udp_t ch_assemble_pool_udp_t;

#include "ch_assemble_pool.h"
#include "ch_assemble_context_udp.h"
#include "ch_app_context_tcp.h"

struct ch_assemble_pool_udp_t {

    ch_assemble_pool_t asp;
    ch_assemble_context_udp_t udp_context;

	ch_app_context_t *app_udp_context;
};

extern ch_assemble_pool_t * ch_assemble_pool_udp_create(ch_context_t *context);


extern void ch_assemble_pool_udp_destroy(ch_assemble_pool_t *asp);


#endif /* CH_ASSEMBLE_POOL_UDP_H */
