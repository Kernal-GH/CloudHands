/*
 *
 *      Filename: ch_packets_interface.h
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-05-31 10:56:54
 * Last Modified: 2017-06-01 10:38:13
 */

#ifndef CH_PACKETS_INTERFACE_H
#define CH_PACKETS_INTERFACE_H


typedef struct ch_packets_interface_t ch_packets_interface_t;

#include <rte_spinlock.h>
#include "ch_packets_context.h"
#include "ch_mmap_packets_format.h"
#include "ch_assemble_packet_tcp.h"
#include "ch_assemble_packet_udp.h"

struct ch_packets_interface_t {
    
	rte_spinlock_t spin_lock;

	ch_packets_context_t *pcontext;

	ch_mmap_packets_format_t *pfmt;


};

#define ch_pin_lock(pin) rte_spinlock_lock(&(pin)->spin_lock)
#define ch_pin_unlock(pin) rte_spinlock_unlock(&(pin)->spin_lock)

extern ch_packets_interface_t * ch_packets_interface_create(apr_pool_t *mp,const char *cfname);

extern void ch_packets_interface_put(ch_packets_interface_t *in,ch_assemble_packet_t *p,int proto_id);

#endif /*CH_PACKETS_INTERFACE_H*/
