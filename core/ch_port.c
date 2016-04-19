/*
 * =====================================================================================
 *
 *       Filename:  ch_port.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时58分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <rte_cycles.h>
#include "ch_constants.h"
#include "ch_log.h"
#include "ch_port.h"

/* allow max jumbo frame 9.5 KB */
#define JUMBO_FRAME_MAX_SIZE	0x2600


static struct rte_eth_conf port_conf = {
	.rxmode = {
		.mq_mode        = ETH_MQ_RX_RSS,
		.max_rx_pkt_len = JUMBO_FRAME_MAX_SIZE,
		.split_hdr_size = 0,
		.header_split   = 0, /**< Header Split disabled */
		.hw_ip_checksum = 1, /**< IP checksum offload enabled */
		.hw_vlan_filter = 0, /**< VLAN filtering disabled */
		.jumbo_frame    = 1, /**< Jumbo Frame Support disabled */
		.hw_strip_crc   = 0, /**< CRC stripped by hardware */
	},
	.rx_adv_conf = {
			.rss_conf = {
				.rss_key = NULL,
				.rss_hf = ETH_RSS_IP,
		},
	},
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	},
};

ch_port_t * ch_port_create(ch_context_t *context,unsigned int port_id){
    
    int ret;

    ch_port_t *port = (ch_port_t*)apr_palloc(context->mp,sizeof(ch_port_t));
    
    port->context = context;

    port->port_id = port_id;
    port->max_pkt_burst = context->port_max_pkt_burst;
    port->nb_rxd = context->port_nb_rxd;
    port->nb_txd = context->port_nb_txd;
    port->n_rxq =  context->port_n_rxq;
    port->n_txq =  context->port_n_txq;

    ret = rte_eth_dev_configure(port_id,port->n_rxq,port->n_txq,&port_conf);

    if(ret<0){

        ch_log(CH_LOG_ERR,"create port[%u] failed",port_id);
        return NULL;
    }

    port->rx_pkts_burst = (struct rte_mbuf**)malloc(port->max_pkt_burst*sizeof(struct rte_mbuf*));

    if(port->rx_pkts_burst == NULL){

        ch_log(CH_LOG_ERR,"no memory create rx pkts burst buffer");
        return NULL;
    }

    port->n_rx_pkts = 0;

    rte_eth_dev_info_get(port_id,&port->dev_info);
    rte_eth_macaddr_get(port_id, &port->addr);
    return port;
}

/* Check the link status of port in up to 9s, and print it finally */
static int
check_port_link_status(ch_port_t *port)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
	uint8_t port_up;
	struct rte_eth_link link;
    int count;

    ch_log(CH_LOG_INFO,"Check link status");

    port_up = 0;
	
    for (count = 0; count <= MAX_CHECK_TIME; count++) {
		
        
        memset(&link, 0, sizeof(link));
        rte_eth_link_get_nowait(port->port_id, &link);
        
        /* print link status if  link up */
        if (link.link_status) {
            port_up = 1;
            break;
        }

		rte_delay_ms(CHECK_INTERVAL);
	}

    if (port_up){
        ch_log(CH_LOG_INFO,"Port %u Link Up - speed %u "
            "Mbps - %s", port->port_id,link.link_speed,
    (link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
        ("full-duplex") : ("half-duplex"));
    
    }

    else{
        ch_log(CH_LOG_INFO,"Port %u Link Down",port->port_id);
    }

    return port_up==1?CH_OK:CH_ERROR;
}

int ch_port_setup(ch_port_t *port,struct rte_mempool *pkt_pool,unsigned int socket){

	struct rte_eth_txconf *txconf;
    unsigned int queue;
    int ret;

    /*setup rx&tx device*/
    for(queue = 0;queue<port->n_rxq;queue++){
        ch_log(CH_LOG_INFO,"Initializing NIC port %u RX&TX queue %u ...",port->port_id,queue);
        
        ret = rte_eth_rx_queue_setup(
                port->port_id,
                queue,
                (uint16_t)port->nb_rxd,
                socket,
                NULL,
                pkt_pool);

        if (ret < 0) {
            ch_log(CH_LOG_ERR,"Cannot init RX queue %u for port %u (%d)",queue,port->port_id,ret);
            return CH_ERROR;
        }

        txconf = &port->dev_info.default_txconf;
        if (port_conf.rxmode.jumbo_frame)
            txconf->txq_flags = 0;

        ret = rte_eth_tx_queue_setup(port->port_id, queue,port->nb_txd,
                         socket, txconf);

        if (ret < 0){
            ch_log(CH_LOG_ERR,"Cannot init TX queue %u for port %u (%d)",queue,port->port_id,ret);
            return CH_ERROR;
        }

        /* Start device */
        ret = rte_eth_dev_start(port->port_id);
        if (ret < 0){
           ch_log(CH_LOG_ERR,"rte_eth_dev_start:err=%d, port=%u",
                          ret, (unsigned) port->port_id);
           return CH_ERROR;
        }

        rte_eth_promiscuous_enable(port->port_id);
    }

    /*check the link status*/
    return check_port_link_status(port);
}

