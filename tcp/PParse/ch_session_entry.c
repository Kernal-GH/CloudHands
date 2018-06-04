/*
 *
 *      Filename: ch_session_entry.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 17:45:37
 * Last Modified: 2018-05-17 19:05:29
 */

#include "ch_session_entry.h"

int ch_session_entry_init(ch_session_entry_t *sentry,ch_pp_parser_t *pparser,ch_tcp_record_t *tcp_record){

	sentry->pp_parser = pparser;
	sentry->session = pparser->session_entry_create(sentry);
	if(sentry->session == NULL)
		return -1;

    sentry->entry.last_time = 0;

    sentry->session_id = tcp_record->session_id;

	if(tcp_record->session_direct == SESSION_DIRECT_REQ){
	
		sentry->req_time = tcp_record->time;
		sentry->res_time = 0;
	}else{
	
		sentry->res_time = tcp_record->time;
		sentry->req_time =0;
	}

    sentry->protocol_id = tcp_record->protocol_id;
    sentry->src_ip = tcp_record->src_ip;
    sentry->dst_ip = tcp_record->dst_ip;
    sentry->src_port = tcp_record->src_port;
    sentry->dst_port = tcp_record->dst_port;

	/*ok*/
	return 0;
}


