/*
 *
 *      Filename: ch_port.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-08-02 01:17:26
 * Last Modified: 2016-08-24 20:47:27
 */

#include "ch_constants.h"
#include "ch_log.h"
#include "ch_port.h"

static inline int _port_enabled(int port_id,unsigned int port_mask){

    return port_mask &(1 << port_id);
}

static inline int _port_id_get(unsigned int port_mask){

	int n_ports;
	int i;

	n_ports = rte_eth_dev_count();
	
	if(n_ports<=0){
	
		ch_log(CH_LOG_ERR,"No port specified to rcv packets!");
		return -1; 
	}

	for(i = 0; i<n_ports; i++){
	
		if(_port_enabled(i,port_mask))
			return i;
	}

	return -1;
}

static inline void _port_rx_conf_init(ch_port_context_t *pcontext,struct rte_eth_rxconf *rx_conf){

	/* Check if any RX parameters have been passed */
	if (pcontext->rx_pthresh != RTE_PMD_PARAM_UNSET)
		rx_conf->rx_thresh.pthresh = pcontext->rx_pthresh;

	if (pcontext->rx_hthresh != RTE_PMD_PARAM_UNSET)
		rx_conf->rx_thresh.hthresh = pcontext->rx_hthresh;

	if (pcontext->rx_wthresh != RTE_PMD_PARAM_UNSET)
		rx_conf->rx_thresh.wthresh = pcontext->rx_wthresh;

	if (pcontext->rx_free_thresh != RTE_PMD_PARAM_UNSET)
		rx_conf->rx_free_thresh = pcontext->rx_free_thresh;

	if (pcontext->rx_drop_en != RTE_PMD_PARAM_UNSET)
		rx_conf->rx_drop_en = pcontext->rx_drop_en;
}

static inline void _port_conf_init(ch_port_t *port,struct rte_eth_conf *port_conf){

	ch_port_context_t *pcontext = port->pcontext;

	port_conf->rxmode.mq_mode        = pcontext->port_n_rxq>1?ETH_MQ_RX_RSS:ETH_MQ_RX_NONE;
	
	port_conf->rxmode.max_rx_pkt_len = CH_MIN(pcontext->jumbo_frame_max_size,port->dev_info.max_rx_pktlen);
	port_conf->rxmode.split_hdr_size = 0;
	port_conf->rxmode.header_split   = 0; /**< Header Split disabled */
	port_conf->rxmode.hw_ip_checksum = 1; /**< IP checksum offload enabled */
	port_conf->rxmode.hw_vlan_filter = 0; /**< VLAN filtering disabled */
	port_conf->rxmode.jumbo_frame    = pcontext->jumbo_frame; /**< Jumbo Frame Support disabled */
	port_conf->rxmode.hw_strip_crc   = 0; /**< CRC stripped by hardware */


	port_conf->rx_adv_conf.rss_conf.rss_key = NULL;

	port_conf->rx_adv_conf.rss_conf.rss_hf = pcontext->port_n_rxq>1?pcontext->rss_hf:0;

	port_conf->txmode.mq_mode = ETH_MQ_TX_NONE;

}
ch_port_t * ch_port_create(ch_context_t *context,unsigned int port_mask){

	ch_port_t *port;
	int port_id,ret;

	port_id = _port_id_get(port_mask);
	
	if(port<0){
	
		return NULL;
	}

    port = (ch_port_t*)apr_pcalloc(context->mp,sizeof(ch_port_t));
    port->context = context;
	port->queues = apr_array_make(context->mp,16,sizeof(ch_port_queue_t*));

	port->pq_index = 0;
    port->port_id = port_id;
	
    rte_eth_dev_info_get(port_id,&port->dev_info);
    rte_eth_macaddr_get(port_id, &port->addr);

	port->rx_conf = port->dev_info.default_rxconf;
	port->pcontext = ch_port_context_create(context);

	if(port->pcontext == NULL){
	
		ch_log(CH_LOG_ERR,"Config port failed:%s",context->port_cfname);
		return NULL;
	}

	_port_conf_init(port,&port->port_conf);

	_port_rx_conf_init(port->pcontext,&port->rx_conf);

    ret = rte_eth_dev_configure(port_id,port->pcontext->port_n_rxq,0,&port->port_conf);

    if(ret<0){

        ch_log(CH_LOG_ERR,"create port[%u] failed",port_id);
        return NULL;
    }



	return port;
}

/* Check the link status of port in up to 9s, and print it finally */
static int check_port_link_status(ch_port_t *port)
{
#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
	uint8_t port_up;
	struct rte_eth_link *link = &port->link;
    int count;

    ch_log(CH_LOG_INFO,"Check link status--------------------------");

    port_up = 0;
	
    for (count = 0; count <= MAX_CHECK_TIME; count++) {
		
        memset(link, 0, sizeof(*link));
        rte_eth_link_get_nowait(port->port_id, link);
        
        /* print link status if  link up */
        if (link->link_status) {
            port_up = 1;
            break;
        }

		rte_delay_ms(CHECK_INTERVAL);
	}

    if (port_up){
        ch_log(CH_LOG_INFO,"Port %u Link Up - speed %u "
            "Mbps - %s", port->port_id,link->link_speed,
    (link->link_duplex == ETH_LINK_FULL_DUPLEX) ?
        ("full-duplex") : ("half-duplex"));
    
    }

    else{
        ch_log(CH_LOG_INFO,"Port %u Link Down",port->port_id);
    }

    return port_up==1?CH_OK:CH_ERROR;
}

static int _port_queue_setup(ch_port_t *port,ch_port_queue_t *pq){

	int ret;

	ret = rte_eth_rx_queue_setup(
			port->port_id,
			pq->pq_id,
			(uint16_t)port->pcontext->port_nb_rxd,
			pq->socket,
			&port->rx_conf,
			pq->pktmbuf_pool);


	if (ret < 0) {
		ch_log(CH_LOG_ERR,"Cannot setup RX queue %u for port %u (%d)",pq->pq_id,port->port_id,ret);
		return -1;
	}

	return 0;
}

static inline int _port_queue_id_get(ch_port_t *port){

	int ret;

	if(port->pq_index>=port->pcontext->port_n_rxq){
	
		ch_log(CH_LOG_ERR,"port:%u queue has been allocated completely!",port->port_id);
		return -1;
	}

	ret = port->pq_index;

	port->pq_index+=1;

	return ret;
}

static inline void _pool_name_init(char *name,size_t name_size,int pq_id){

	snprintf(name,name_size,"rx_pktmbuf_%d",pq_id);
}

static ch_port_queue_t * _port_queue_create(ch_port_t *port,unsigned int socket){

	char pkmbuf_name[64] = {0};

	ch_port_context_t *pcontext = port->pcontext;

	int pq_id = _port_queue_id_get(port);
	if(pq_id<0){
	
		return NULL;
	}
	
	ch_port_queue_t *pq = (ch_port_queue_t*)apr_palloc(port->context->mp,sizeof(ch_port_queue_t));

    pq->rx_pkts_burst = (struct rte_mbuf**)malloc(pcontext->port_max_pkt_burst*sizeof(struct rte_mbuf*));

    if(pq->rx_pkts_burst == NULL){

        ch_log(CH_LOG_ERR,"no memory create rx pkts burst buffer");
        return NULL;
    }
	
	pq->port = port;
	pq->socket = socket;
	pq->pq_id = pq_id;

	_pool_name_init(pkmbuf_name,64,pq_id);

    /*alloc packet pool */
    pq->pktmbuf_pool = rte_mempool_create(pkmbuf_name,
            pcontext->n_rx_mbuf,
            pcontext->rx_mbuf_size,
            pcontext->rx_mbuf_cache_size,
			sizeof(struct rte_pktmbuf_pool_private),
			rte_pktmbuf_pool_init, NULL, rte_pktmbuf_init, NULL,
            socket,MEMPOOL_F_SC_GET
            );

    if(pq->pktmbuf_pool == NULL){
        ch_log(CH_LOG_ERR,"create packet mbuf pool failed for port:%u,port_queue:%d!",port->port_id,pq->pq_id);
       
		free((void*)pq->rx_pkts_burst);
		pq->rx_pkts_burst = NULL;

		return NULL;
    }

	return pq;
}

static inline void _port_queue_destroy(ch_port_queue_t *pq){

	if(pq->rx_pkts_burst)
	{
	
		free((void*)pq->rx_pkts_burst);
		pq->rx_pkts_burst = NULL;
	}
} 

ch_port_queue_t * ch_port_queue_bind(ch_port_t *port,unsigned int socket){

	ch_port_queue_t *pq = _port_queue_create(port,socket);

	if(pq == NULL)
		return NULL;


	if(_port_queue_setup(port,pq)<0)
	{
		
		_port_queue_destroy(pq);

		return NULL;
	
	}
	/* add to arrs */
    *(ch_port_queue_t**)apr_array_push(port->queues) = pq;

	return pq;
}


int ch_port_setup(ch_port_t *port){

	int ret;

	/* Start device */
	ret = rte_eth_dev_start(port->port_id);
	
	if (ret < 0){
	   ch_log(CH_LOG_ERR,"rte_eth_dev_start:err=%d, port=%u",
					  ret, (unsigned) port->port_id);
	   return CH_ERROR;
	}

	rte_eth_promiscuous_enable(port->port_id);

    /*check the link status*/
    return check_port_link_status(port);

}

void ch_port_destroy(ch_port_t *port){

	unsigned int i,n;
	ch_port_queue_t *pq,**arr;

	n = port->queues->nelts;

	arr = (ch_port_queue_t**)port->queues->elts;

	for(i = 0; i<n;i++){
	
		pq = arr[i];

		_port_queue_destroy(pq);
	}
}
