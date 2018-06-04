/*
 * =====================================================================================
 *
 *       Filename:  ch_udp.c
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

#include "ch_log.h"
#include "ch_protocol_type.h"
#include "ch_util.h"
#include "ch_udp.h"
#include "ch_assemble_packet_udp.h"
#include "ch_assemble_pool_udp.h"

static inline void _udp_packet_init(ch_assemble_packet_udp_t *udp_pkt,struct rte_mbuf *mbuf){

	struct ipv4_hdr *iph = ch_ipv4_hdr_get(mbuf);
	ch_assemble_packet_t *as_pkt = (ch_assemble_packet_t*)udp_pkt;
    
	as_pkt->mbuf = mbuf;
    as_pkt->src_ip = iph->src_addr; 
    as_pkt->dst_ip = iph->dst_addr;

}

static int udp_rcv(ch_protocol_type_t *protype,struct rte_mbuf *mbuf){
    
    ch_assemble_packet_udp_t udp_pkt;

    ch_udp_t *udp = (ch_udp_t *)protype->proto_type_priv;

	_udp_packet_init(&udp_pkt,mbuf);
   

    return ch_assemble_pool_packet_process(udp->asp,(ch_assemble_packet_t *)&udp_pkt);

}

ch_protocol_type_t udp_proto = {

    CH_IPPROTO_UDP,
    NULL,
    udp_rcv
};

ch_udp_t * ch_udp_create(ch_context_t *context,ch_ipv4_t *ipv4){

    ch_udp_t *udp = (ch_udp_t*)apr_palloc(context->mp,sizeof(ch_udp_t));

    if(!udp){

        ch_log(CH_LOG_ERR,"no memory to create udp instance!");
        return NULL;
    }

    udp->context = context;

    udp_proto.proto_type_priv = (void*)udp;

    /*register udp protocol into ipv4,*/
    ch_ipv4_proto_register(ipv4,&udp_proto);

    udp->asp = ch_assemble_pool_udp_create(context);

    if(udp->asp == NULL){
        ch_log(CH_LOG_ERR,"Cannot create udp assemble pool for udp!");
        return NULL;
    }
    
	return udp;
}
