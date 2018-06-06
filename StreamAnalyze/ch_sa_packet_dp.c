/*
 *
 *      Filename: ch_sa_packet_dp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-08 11:20:32
 * Last Modified: 2018-06-06 12:05:41
 */

#include "ch_sa_packet_dp.h"
#include "ch_sa_icmp_handler.h"
#include "ch_sa_arp_handler.h"
#include "ch_sa_tcp_session_request_handler.h"
#include "ch_sa_udp_session_handler.h"

int ch_sa_packet_dp(ch_sa_session_task_t *sa_task,ch_packet_t *pkt){

	ch_stat_pool_t *st_pool = sa_task->sa_work->st_pool;
	uint64_t pkt_size = ch_packet_size(pkt);
	uint64_t time;
	int stat_pkt_type = STAT_OTHER;

	switch(pkt->pkt_type){
	
		case PKT_TYPE_TCP:
			stat_pkt_type = STAT_TCP;
			ch_sa_tcp_session_request_packet_handle(sa_task->tcp_req_handler,pkt);
			break;

		case PKT_TYPE_UDP:
			stat_pkt_type = STAT_UDP;
			ch_sa_udp_session_packet_handle(sa_task->udp_handler,pkt);
			break;

		case PKT_TYPE_ARP:

			stat_pkt_type = STAT_ARP;
			ch_sa_arp_handle(sa_task,pkt);
			break;

		case PKT_TYPE_ICMP:

			stat_pkt_type = STAT_ICMP;
			ch_sa_icmp_handle(sa_task,pkt);
			break;
		default:
			break;

	}


	ch_packet_free(pkt);

	time = ch_get_current_timems()/1000;

	ch_stat_pool_handle(st_pool,time,pkt_size,stat_pkt_type);

	return 0;
}

