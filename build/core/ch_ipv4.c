/*
 * =====================================================================================
 *
 *       Filename:  ch_ipv4.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月27日 11时12分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <rte_ip.h>
#include "ch_constants.h"
#include "ch_ipv4.h"
#include "ch_packet_type.h"
#include "ch_packet.h"
#include "ch_ethertype.h"
#include "ch_log.h"
#include "ch_util.h"
#include "ch_protocol_type.h"
#include "ch_ethertype.h"

#define IPV(hdr) ((((hdr)->version_ihl)&0xf0)>>4)
#define IPHL(hdr) (((hdr)->version_ihl)&0x0f)

static int ipv4_rcv(ch_packet_type_t *ptype,ch_packet_info_t *pinfo){

    ch_protocol_type_t *proto;

    struct ipv4_hdr *ip_hdr = ch_ipv4_hdr_get(pinfo->mbuf);

    /*check ip header len,and version */
    if(IPHL(ip_hdr)<5||IPV(ip_hdr)!=4){
        ch_log(CH_LOG_ERR,"invalid ip header!");
        return PROCESSOR_RET_DROP;
    }

    pinfo->mbuf->l3_len = IPHL(ip_hdr)*4;

    /*get up proto*/
    proto = ch_ipv4_proto_get((ch_ipv4_t*)ptype->packet_type_priv,ip_hdr->next_proto_id);

    /*if no uplevel protocol registered to handle this packet,drop it*/
    if(proto == NULL){
        ch_log(CH_LOG_DEBUG,"no uplevel protocol[%d] has been registered to handle this packet!",ip_hdr->next_proto_id);
        return PROCESSOR_RET_DROP;
    }

    ch_packet_ipinfo_init(pinfo,ip_hdr);
    /*call uplevel protocol to handle this packet*/
    return proto->handler(proto,pinfo); 
}

ch_packet_type_t ipv4_ptype = {
    {NULL,NULL},
    CH_ETH_P_IP,
    NULL,
    ipv4_rcv
};


ch_ipv4_t * ch_ipv4_create(ch_context_t *context,ch_ethertype_t *ethertype){
    
    int i;

    ch_ipv4_t *ipv4 = (ch_ipv4_t*)apr_palloc(context->mp,sizeof(ch_ipv4_t));

    if(!ipv4){
        ch_log(CH_LOG_ERR,"no enough memory to create ipv4 instance!");
        return NULL;
    }

    for(i = 0 ; i < CH_IPPROTO_MAX; i++){
        ipv4->protos[i] = NULL;
    }
    ipv4->context = context;
    ipv4_ptype.packet_type_priv = (void*)ipv4;
    /*register ip protocol into ethertype to rcv ip packet*/
    ch_ethertype_packet_type_register(ethertype,&ipv4_ptype);

    return ipv4;
}



