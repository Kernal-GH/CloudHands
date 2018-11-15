/*
 *
 *      Filename: ch_process_interface_udp_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-02-06 10:31:14
 * Last Modified: 2018-02-06 10:31:14
 */

#ifndef CH_PROCESS_INTERFACE_UDP_CONTEXT_H
#define CH_PROCESS_INTERFACE_UDP_CONTEXT_H

typedef struct ch_process_interface_udp_context_t ch_process_interface_udp_context_t;

#include "ch_mpool.h"
#include "ch_process_interface.h"
#include "ch_session_monitor.h"
#include "ch_wb_list.h"
#include "ch_wb_list_ip.h"

#define MAX_PORT_ARRAY_SIZE 1024

struct ch_process_interface_udp_context_t {

	ch_pool_t *mp;
	
	ch_wb_list_t *ip_white_list;
	ch_wb_list_t *ip_black_list;

	ch_process_interface_t *pint;
	ch_session_monitor_t monitor;
	const char *pool_name;

	const char *qprefix;

	uint32_t qnumber;

	uint32_t qsize;

	uint16_t accept_ports[MAX_PORT_ARRAY_SIZE];
	
	const char *smon_mmap_fname;
	size_t smon_mmap_fsize;
};

extern ch_process_interface_udp_context_t *ch_process_interface_udp_context_create(ch_pool_t *mp,const char *cfname,int is_write);

#endif /*CH_PROCESS_INTERFACE_UDP_CONTEXT_H*/
