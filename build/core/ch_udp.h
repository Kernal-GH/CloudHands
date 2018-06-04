/*
 * =====================================================================================
 *
 *       Filename:  ch_udp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月28日 9时44分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_UDP_H
#define CH_UDP_H

typedef struct ch_udp_t ch_udp_t;

#include "ch_context.h"
#include "ch_ipv4.h"
#include "ch_assemble_pool.h"

struct ch_udp_t {

	ch_context_t *context;

	ch_assemble_pool_t *asp;
}; 


extern ch_udp_t * ch_udp_create(ch_context_t *context,ch_ipv4_t *ipv4);

extern void ch_udp_destroy(ch_udp_t *tcp);

#endif /*CH_UDP_H*/
