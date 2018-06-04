/*
 * =====================================================================================
 *
 *       Filename:  ch_packet_type.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月25日 13时19分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_PACKET_TYPE_H
#define CH_PACKET_TYPE_H
#include "ch_list.h"
#include <rte_mbuf.h>

typedef struct ch_packet_type_t ch_packet_type_t;

#include <apr_ring.h>

struct ch_packet_type_t {
    struct ch_list_head link;
    uint16_t type;
    void *packet_type_priv;
    int (*fun)(ch_packet_type_t *ptype,struct rte_mbuf *mbuf);
};

#endif /*CH_PACKET_TYPE_H*/

