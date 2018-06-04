/*
 *
 *      Filename: ch_sa_packet_dp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-08 11:20:32
 * Last Modified: 2018-04-20 18:11:13
 */

#include "ch_sa_packet_dp.h"
#include "ch_sa_icmp_handler.h"
#include "ch_sa_arp_handler.h"
#include "ch_sa_tcp_session_request_handler.h"
#include "ch_sa_udp_session_handler.h"

int ch_sa_packet_dp(ch_sa_session_task_t *sa_task,ch_packet_t *pkt){


	switch(pkt->pkt_type){
	
		case PKT_TYPE_TCP:
			ch_sa_tcp_session_request_packet_handle(sa_task->tcp_req_handler,pkt);
			break;

		case PKT_TYPE_UDP:
			ch_sa_udp_session_packet_handle(sa_task->udp_handler,pkt);
			break;

		case PKT_TYPE_ARP:
			ch_sa_arp_handle(sa_task,pkt);
			break;

		case PKT_TYPE_ICMP:
			ch_sa_icmp_handle(sa_task,pkt);
			break;
		default:
			break;

	}


	ch_packet_free(pkt);


	return 0;
}

