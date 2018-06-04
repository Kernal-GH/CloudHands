/*
 *
 *      Filename: ch_proto_tcp.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 20:51:17
 * Last Modified: 2016-10-27 20:51:17
 */

#ifndef CH_PROTO_TCP_H
#define CH_PROTO_TCP_H

typedef struct ch_proto_tcp_t ch_proto_tcp_t;

#include "ch_proto_pool.h"

struct ch_proto_tcp_t {

	ch_proto_trans_t trans_proto;

};

int ch_proto_tcp_init(ch_proto_pool_t *ppool);

#endif /* CH_PROTO_TCP_H */
