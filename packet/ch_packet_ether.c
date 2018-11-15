/*
 *
 *      Filename: ch_packet_ether.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-12 16:11:28
 * Last Modified: 2018-04-21 17:23:58
 */

#include "ch_packet_ether.h"
#include "ch_log.h"
#include <rte_ether.h>

#define IS_PASS(eth) (is_valid_assigned_ether_addr(&eth->d_addr)&&is_valid_assigned_ether_addr(&eth->s_addr)?0:1)

int ch_packet_ether_parse(ch_packet_t *pkt){

	const struct ether_hdr *eh;
	struct ether_hdr eh_copy;
	uint32_t off = 0;
	uint16_t proto;
	struct rte_mbuf *mbuf = pkt->mbuf;

	eh = rte_pktmbuf_read(mbuf, off, sizeof(*eh), &eh_copy);
	if (unlikely(eh == NULL))
	{
		ch_log(CH_LOG_DEBUG,"Invalid Ethernet packet,discard it!");
		return PKT_PARSE_DROP;
	}

#if 0
	if(IS_PASS(eh)){
	
		ch_log(CH_LOG_DEBUG,"Discard multibroad packet!");
		return PKT_PARSE_DROP;
	}
#endif 

	proto = rte_be_to_cpu_16(eh->ether_type);

    /*Invalid ethernet packet!!*/
    if(proto>=CH_ETH_LENGTH_MAX&&proto<=CH_ETH_P_802_3_MIN){
        ch_log(CH_LOG_DEBUG,"Invalid ethernet packet,ethtype[%u]!",proto);
        return PKT_PARSE_DROP;
    }

    /*Ignore 802.3 packets!*/
    if(proto<CH_ETH_LENGTH_MAX){

        ch_log(CH_LOG_DEBUG,"Ignore ethernet 802.3 packet,ethtype[%u]",proto);
        return PKT_PARSE_DROP;
    }
	
	off = sizeof(*eh);

    /*EthernetII*/

	do{
		if (proto == CH_ETH_P_8021Q){
			/*8021q vlan */
			const struct vlan_hdr *vh;
			struct vlan_hdr vh_copy;

			//ch_log(CH_LOG_DEBUG,"802.1q!\n");
			vh = rte_pktmbuf_read(mbuf, off, sizeof(*vh), &vh_copy);
			if(vh == NULL)
				break;

			off += sizeof(*vh);
			proto = rte_be_to_cpu_16(vh->eth_proto);
		}else if (proto == CH_ETH_P_8021AD) {
			const struct vlan_hdr *vh;
			struct vlan_hdr vh_copy;

			//ch_log(CH_LOG_DEBUG,"8021ad!\n");
			vh = rte_pktmbuf_read(mbuf, off + sizeof(*vh), sizeof(*vh),
				&vh_copy);
			if(vh == NULL)
				break;

			off += 2 * sizeof(*vh);
			proto = rte_be_to_cpu_16(vh->eth_proto);
		}
	}while(proto == CH_ETH_P_8021Q||proto == CH_ETH_P_8021AD);

	pkt->l2_len = off;
	pkt->parse_off = off;
	pkt->l3_proto = proto;

    /*OK!*/
    return PKT_PARSE_OK; 
}

