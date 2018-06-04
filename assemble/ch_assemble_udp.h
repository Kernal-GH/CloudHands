/*
 *
 *      Filename: ch_assemble_udp.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-14 18:10:48
 * Last Modified: 2016-10-14 18:10:48
 */

#ifndef CH_ASSEMBLE_UDP_H
#define CH_ASSEMBLE_UDP_H

typedef struct ch_assemble_udp_t ch_assemble_udp_t;

#include <rte_ip_frag.h>
#include "ch_assemble.h"
#include "ch_assemble_pool_udp.h"

struct ch_assemble_udp_t {

    ch_assemble_t as;

    ch_assemble_context_udp_t *udp_context;
    
	ch_assemble_pool_udp_t *aspu;

	struct rte_ip_frag_death_row dr;

	struct rte_ip_frag_tbl *frag_tbl;
};

extern ch_assemble_t * ch_assemble_udp_create(ch_assemble_pool_udp_t *aspu,int assemble_id);

extern void ch_assemble_udp_destroy(ch_assemble_udp_t *uas);

#endif /* CH_ASSEMBLE_UDP_H */
