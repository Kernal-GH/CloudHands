/*
 *
 *      Filename: ch_udp_session.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-10 18:43:10
 * Last Modified: 2018-07-16 17:23:08
 */

#include "ch_udp_session.h"

int ch_udp_session_init(ch_udp_session_t *udp_session,ch_packet_udp_t *pkt_udp,ch_udp_app_session_t *app_session,
	uint64_t session_id){

    ch_udp_session_endpoint_t *req = &udp_session->endpoint_req;
    ch_udp_session_endpoint_t *res = &udp_session->endpoint_res;

	uint64_t time = ch_get_current_timems();

	udp_session->session_id = session_id;
	udp_session->app_session = app_session;
    udp_session->is_ipv6 = pkt_udp->is_ipv6;

    if(pkt_udp->is_ipv6){

        memcpy(req->addr,pkt_udp->src_addr,16);
        memcpy(res->addr,pkt_udp->dst_addr,16);
    }
	/*init req endpoint*/
	req->ip = pkt_udp->src_ip;
	req->port = pkt_udp->src_port;
	req->packets = 1;
	req->bytes = pkt_udp->payload_len;
	req->start_time = time;
	req->last_time = time;

	/*init res endpoint*/
	res->ip = pkt_udp->dst_ip;
	res->port = pkt_udp->dst_port;
	res->packets = 0;
	res->bytes = 0;
	res->start_time = time;
	res->last_time = time;

	return 0;
}

