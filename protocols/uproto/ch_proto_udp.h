/*
 *
 *      Filename: ch_proto_udp.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-30 19:14:25
 * Last Modified: 2016-10-30 19:14:25
 */

#ifndef CH_PROTO_UDP_H
#define CH_PROTO_UDP_H

typedef struct ch_proto_udp_t ch_proto_udp_t;

#include "ch_proto_pool.h"

struct ch_proto_udp_t {

	ch_proto_trans_t trans_proto;

};

int ch_proto_udp_init(ch_proto_pool_t *ppool);

#endif /* CH_PROTO_UDP_H */
