/*
 *
 *      Filename: ch_sa_packet_dp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-08 11:20:32
 * Last Modified: 2018-08-01 12:02:11
 */

#include "ch_sa_packet_dp.h"
#include "ch_sa_icmp_handler.h"
#include "ch_sa_arp_handler.h"
#include "ch_sa_tcp_session_request_handler.h"
#include "ch_sa_udp_session_handler.h"

int ch_sa_packet_dp(ch_sa_session_task_t *sa_task,ch_packet_t *pkt){

    ch_sa_context_t *context = sa_task->sa_work->sa_context;

	switch(pkt->pkt_type){
	
		case PKT_TYPE_TCP:

            if(context->tcp_sa_on){
                ch_sa_tcp_session_request_packet_handle(sa_task->tcp_req_handler,pkt);
            }

			break;

		case PKT_TYPE_UDP:

            if(context->udp_sa_on){
			
                ch_sa_udp_session_packet_handle(sa_task->udp_handler,pkt);
            }

            break;

		case PKT_TYPE_ARP:

            if(context->arp_sa_on){
			
                ch_sa_arp_handle(sa_task,pkt);
            }

			break;

		case PKT_TYPE_ICMP:

            if(context->icmp_sa_on){
			
                ch_sa_icmp_handle(sa_task,pkt);
            }

			break;
		default:

			break;

	}


	//ch_packet_free(pkt);


	return 0;
}

