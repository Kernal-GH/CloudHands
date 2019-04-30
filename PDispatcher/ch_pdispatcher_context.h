/*
 * =====================================================================================
 *
 *       Filename:  ch_pdispatcher_context.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/06/2017 02:53:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_PDISPATCHER_CONTEXT_H
#define CH_PDISPATCHER_CONTEXT_H

typedef struct ch_pdispatcher_context_t ch_pdispatcher_context_t;

#include "ch_log.h"
#include "ch_mpool.h"
#include "ch_core_pool.h"
#include "ch_port_pool.h"
#include "ch_packet_rxtask_pool.h"
#include "ch_process_interface_tcp_context.h"
#include "ch_process_interface_sa_context.h"
#include "ch_process_interface_udp_context.h"
#include "ch_wb_list.h"
#include "ch_wb_list_ip.h"
#include "ch_stat_pool.h"
#include "ch_redis_ip_wblist.h"

#define LOG_NAME_DEFAULT "/tmp/pdispatcher.log"
#define LOG_LEVEL_DEFAULT CH_LOG_NOTICE
#define PORT_CFNAME_DEFAULT "/usr/local/dpdk/CloudHands/conf/port.conf"
#define PINT_TCP_CFNAME_DEFAULT "/usr/local/dpdk/CloudHands/conf/tcp/TCPPint.conf"
#define PINT_SA_CFNAME_DEFAULT "/usr/local/dpdk/CloudHands/conf/sa/SAPint.conf"
#define PINT_UDP_CFNAME_DEFAULT "/usr/local/dpdk/CloudHands/conf/udp/UDPPint.conf"

struct ch_pdispatcher_context_t {

   ch_pool_t *mp;

   ch_core_pool_t *cpool;

   ch_port_pool_t *ppool;

   ch_packet_rxtask_pool_t *rxtask_pool;

   ch_wb_list_t ip_white_list;
   ch_wb_list_t ip_black_list;

   ch_redis_ip_wblist_t *ip_wblist;

   ch_process_interface_tcp_context_t *pint_tcp_context;
   
   ch_process_interface_sa_context_t  *pint_sa_context;
   
   ch_process_interface_udp_context_t  *pint_udp_context;
   
   ch_stat_pool_t *st_pool;

   /*config log name and level*/
   const char *log_name;
   int log_level;
   

   /*config port config file path*/
   const char *port_cfname;

   /*config port mask*/
   uint32_t port_mask;

   /*config packet receive task's number*/
   uint32_t packet_rxtasks_n;

   /*config process interface tcp  config file path*/
   const char *pint_tcp_cfname;

   /*Config process interface stream analyze config file path*/
   const char *pint_sa_cfname;

   /*Config process interface udp config file path*/
   const char *pint_udp_cfname;

   /*config ip white list */
   const char *ip_wlist_mmap_fname;
   size_t ip_wlist_msize;

   /*config ip black list */
   const char *ip_blist_mmap_fname;
   size_t ip_blist_msize;

   const char *redis_ipwblist_fname;

   /*statistic configs*/
	const char *stat_mmap_fname;
	uint64_t stat_time_up;
	uint64_t stat_time_tv;

};


extern ch_pdispatcher_context_t * ch_pdispatcher_context_create(ch_pool_t *mp,const char *cfname);

extern int ch_pdispatcher_context_start(ch_pdispatcher_context_t *pdcontext);

extern void ch_pdispatcher_context_stop(ch_pdispatcher_context_t *pdcontext);

#endif /*CH_PDISPATCHER_CONTEXT_H*/

