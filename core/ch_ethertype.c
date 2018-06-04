/*
 * =====================================================================================
 *
 *       Filename:  ch_ethertype.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年01月25日 16时04分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_constants.h"
#include "ch_log.h"
#include "ch_ethertype.h"

/*pass this packet,if packet is not a unicast packet */
static inline int is_pass(struct ether_hdr *eth){

    return is_valid_assigned_ether_addr(&eth->d_addr)&&is_valid_assigned_ether_addr(&eth->s_addr)?0:1;
}


static int parse_ethernet_header(struct rte_mbuf *mbuf,struct ether_hdr *eth){

	uint16_t proto = rte_be_to_cpu_16(eth->ether_type);
	uint32_t off = 0;

    /*Invalid ethernet packet!!*/
    if(proto>=CH_ETH_LENGTH_MAX&&proto<=CH_ETH_P_802_3_MIN){
        ch_log(CH_LOG_DEBUG,"Invalid ethernet packet,ethtype[%u]!",proto);
        return -1;
    }

    /*Ignore 802.3 packets!*/
    if(proto<CH_ETH_LENGTH_MAX){

        ch_log(CH_LOG_DEBUG,"Ignore ethernet 802.3 packet,ethtype[%u]",proto);
        return -1;
    }

    /*EthernetII*/
    //mbuf->l2_len = sizeof(struct ether_hdr);
	off+=sizeof(struct ether_hdr);

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

	mbuf->l2_len = off;

    eth->ether_type = proto;

    /*OK!*/
    return 0; 
}

static int ethertype_rx_task_process(struct rte_mbuf *mbuf,uint64_t time,void *priv_data){

    uint16_t type;
    ch_packet_type_t *ptype;

    ch_ethertype_t *ethertype = (ch_ethertype_t*)priv_data;
    struct ether_hdr *eth;

    eth = rte_pktmbuf_mtod(mbuf,struct ether_hdr *);

    if(parse_ethernet_header(mbuf,eth)){
        /*failed!*/
        //ch_log(CH_LOG_DEBUG,"tx ethernet header parse failed!");
        return PROCESSOR_RET_DROP;
    }

    /*accept only unicast packet*/
    if(is_pass(eth)){
        //ch_log(CH_LOG_DEBUG,"tx ethernet header parse unicast packet drop!");
        return PROCESSOR_RET_DROP;
    }

    type = eth->ether_type;

    ptype = ch_ethertype_packet_type_get(ethertype,type);
    
    /*no found uplevel protocol to process this packet ,discard it*/
    if(ptype == NULL || ptype->fun == NULL){
        //ch_log(CH_LOG_DEBUG,"tx ethernet no found uplevel!, type=%d.\n", type);
        return PROCESSOR_RET_DROP;
    }
    
    /*Call up level protocol to process this packet*/
    return ptype->fun(ptype,mbuf); 
}

ch_rxtask_processor_t rx_processor = {
    "",
    ethertype_rx_task_process,
    NULL
};

ch_ethertype_t * ch_ethertype_create(ch_context_t *context,ch_rxtask_pool_t *rxtp){
   
    int i;
    ch_ethertype_t *ethertype = (ch_ethertype_t*)apr_palloc(context->mp,sizeof(ch_ethertype_t));
    if(ethertype == NULL){
        ch_log(CH_LOG_ERR,"no space create ethertype instance!");
        return NULL;
    }

    for(i=0;i<CH_ETH_ETHERTYPE_HASH_SIZE;i++){

        CH_INIT_LIST_HEAD(&ethertype->packet_types[i]);
    }

    ethertype->context =  context;
    rx_processor.priv_data = (void*)ethertype;
    ch_rxtask_pool_pkt_processor_register(rxtp,&rx_processor);

    return ethertype;
}

void ch_ethertype_packet_type_register(ch_ethertype_t *ethertype,ch_packet_type_t *ptype){

    struct ch_list_head * head = ch_ethertype_packet_types_head_get(ethertype,ptype->type);
    
    /*check ptype if existed*/
    if(ch_ethertype_packet_type_get(ethertype,ptype->type)){
        /*ignore*/
        ch_log(CH_LOG_WARN,"packet type has registered,igore it!");
        return;
    }

    /*add this ptype into list tail*/
    list_add_tail(&ptype->link,head);
}

ch_packet_type_t * ch_ethertype_packet_type_get(ch_ethertype_t *ethertype,uint16_t type){

    ch_packet_type_t *ptype;

    struct ch_list_head *head = ch_ethertype_packet_types_head_get(ethertype,type);
    
    list_for_each_entry(ptype,head,link){
        
        if(ptype->type == type)
            return ptype;
    }

    return NULL;
}

