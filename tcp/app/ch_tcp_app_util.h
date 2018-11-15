/*
 *
 *      Filename: ch_tcp_app_util.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 16:58:37
 * Last Modified: 2018-07-12 16:58:37
 */

#ifndef CH_TCP_APP_UTIL_H
#define CH_TCP_APP_UTIL_H

#include "ch_tcp_record.h"
#include "ch_tcp_session.h"
#include "ch_tcp_app_pool.h"

#if 0
static inline void ch_tcp_record_init(ch_tcp_record_t *tcp_rcd,ch_tcp_session_t *tcp_session,
	uint8_t packet_type,void *data,size_t dlen,uint32_t protocol_id,uint8_t direct){

	tcp_rcd->shm_rcd.magic = EIMAGIC;

	tcp_rcd->shm_rcd.record_size = CH_TCP_RECORD_HEADER_SIZE+dlen;

	tcp_rcd->shm_rcd.data_offset = tcp_session->cur_ep->last_offset-dlen;

	tcp_rcd->shm_rcd.data_len = dlen;
	tcp_rcd->shm_rcd.data = data; 

	tcp_rcd->packet_type = packet_type;
	tcp_rcd->session_direct = direct;
	tcp_rcd->protocol_id = protocol_id;
	tcp_rcd->session_id = tcp_session->session_id;
	tcp_rcd->time = tcp_session->cur_ep->time;
	tcp_rcd->src_ip = ch_tcp_session_srcip_get(tcp_session);
	tcp_rcd->dst_ip = ch_tcp_session_dstip_get(tcp_session);
	tcp_rcd->src_port = ch_tcp_session_srcport_get(tcp_session);
	tcp_rcd->dst_port = ch_tcp_session_dstport_get(tcp_session);

	tcp_rcd->req_packets = ch_tcp_session_src_packets_get(tcp_session);
	tcp_rcd->req_bytes = ch_tcp_session_src_bytes_get(tcp_session);
	tcp_rcd->res_packets = ch_tcp_session_dst_packets_get(tcp_session);
	tcp_rcd->res_bytes = ch_tcp_session_dst_bytes_get(tcp_session);
	
}
#endif

static inline void ch_dump_ports(uint16_t* ports,const char *pre,int port_n){

	int i;
	uint16_t port;

	fprintf(stdout,"%s ports:",pre);

	for(i = 0; i<port_n;i++){
	
		port = ports[i];

		if(port == 0)
			break;
		fprintf(stdout,"%ld,",(unsigned long)port);
	}

	fprintf(stdout,"\n\n");
}

static inline const char *ch_config_ports(uint16_t *ports,int argc,char *const argv[],int port_max_n){

    char *endptr;
	int i;
	uint16_t port;

    if(argc>port_max_n){
    
        return "The ports is too much!!";
    }
    
	for(i = 0; i<argc;i++){
	
		port = (uint16_t)strtoul(argv[i],&endptr,10);
		if(port == 0){
		
			return "The port is zero!";
		}
		
		ports[i] = port;
	}

    return NULL;
}

#endif /*CH_TCP_APP_UTIL_H*/
