/*
 * =====================================================================================
 *
 *       Filename:  ch_port_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2017 11:18:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_log.h"
#include "ch_port_pool.h"
#include "ch_packet.h"

static inline int _port_enabled(int port_id,uint32_t port_mask){

    return port_mask &(1 << port_id);
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

static inline void _port_conf_init(ch_port_t *port,ch_port_context_t *pcontext,struct rte_eth_conf *port_conf){


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

static inline int _get_port_queue_number(ch_port_t *port,ch_port_context_t *pcontext){

   uint16_t max_port_queue = port->dev_info.max_rx_queues;
   if(pcontext->port_n_rxq<=1||max_port_queue<=1)
      return 1;

   return CH_MIN(pcontext->port_n_rxq,max_port_queue);
}

static inline void _pool_name_init(ch_port_pool_t *ppool,char *name,size_t name_size){

	snprintf(name,name_size,"rx_pktmbuf_%d",ppool->port_queue_c);
	ppool->port_queue_c+=1;
}

static ch_port_queue_t * _port_queue_create(ch_port_pool_t *ppool,ch_port_t *port,int queue_id){


	ch_port_queue_t *pq = (ch_port_queue_t*)ch_palloc(ppool->mp,sizeof(ch_port_queue_t));

	pq->port = port;
	pq->queue_id  = queue_id;

	return pq;
}

static ch_port_t * _create_port(ch_port_pool_t *ppool,ch_port_context_t *pcontext,int port_id){

   int ret;
   int queue_id = 0;
   ch_port_t *port = NULL;
   ch_port_queue_t *pq;

   port = (ch_port_t*)ch_pcalloc(ppool->mp,sizeof(*port));
   port->ppool = ppool;
   port->port_id = port_id;
   port->cur_port_queue_index = 0;
   port->port_queues = ch_array_make(ppool->mp,16,sizeof(ch_port_queue_t*));

   rte_eth_dev_info_get(port_id,&port->dev_info);
   rte_eth_macaddr_get(port_id, &port->addr);

   port->rx_conf = port->dev_info.default_rxconf;
   
   _port_conf_init(port,pcontext,&port->port_conf);
   
   _port_rx_conf_init(pcontext,&port->rx_conf);

   port->port_queue_n = _get_port_queue_number(port,pcontext);

   ret = rte_eth_dev_configure(port_id,port->port_queue_n,0,&port->port_conf);

    if(ret<0){

        ch_log(CH_LOG_ERR,"create port[%u] failed",port_id);
        return NULL;
    }

    /*create all queues*/
    for(queue_id = 0; queue_id<port->port_queue_n;queue_id++){
   
       pq = _port_queue_create(ppool,port,queue_id);
       /* add to port */
       *(ch_port_queue_t**)ch_array_push(port->port_queues) = pq;

	   ppool->port_queue_n+=1;
    }

	return port;
}

ch_port_pool_t * ch_port_pool_create(ch_pool_t *mp,const char *cfname,const char *pkt_pool_name,const char *sa_pool_name,uint32_t port_mask){

   ch_port_pool_t * ppool = NULL;
   ch_port_context_t *pcontext;
   ch_port_t *port;
   int port_id;
   int port_n;

   port_n = rte_eth_dev_count();
   if(port_n<=0){
   
      ch_log(CH_LOG_ERR,"No dpdk port configged to rcv packets,please config it by dpdk script!");
      return NULL;
   }

   /* create port config context*/
   pcontext = ch_port_context_create(mp,cfname);
   if(pcontext == NULL){
   
      ch_log(CH_LOG_ERR,"Create port pool config context failed:%s",cfname);
      return NULL;
   }

   ppool = (ch_port_pool_t*)ch_pcalloc(mp,sizeof(*ppool));
   ppool->mp = mp;
   ppool->pcontext = pcontext;
   ppool->ports = ch_array_make(mp,16,sizeof(ch_port_t*));
   ppool->port_n = 0;
   ppool->cur_port_index = 0;
   ppool->port_queue_c = 0;
   ppool->port_queue_n = 0;
	
   /*create mbuf pool */	
   ppool->pktmbuf_pool = rte_pktmbuf_pool_create(pkt_pool_name,
												  pcontext->n_rx_mbuf,
		                                          pcontext->rx_mbuf_cache_size,
												  RTE_ALIGN(sizeof(ch_packet_t),RTE_MBUF_PRIV_ALIGN),
												  pcontext->rx_mbuf_size, 
												  rte_socket_id());

    if(ppool->pktmbuf_pool == NULL){
        ch_log(CH_LOG_ERR,"create packet mbuf pool failed!");
       
		return NULL;
    }
  
#if 0
	ppool->sambuf_pool = rte_pktmbuf_pool_create(sa_pool_name,
												  pcontext->n_rx_mbuf,
		                                          pcontext->rx_mbuf_cache_size,
												  RTE_ALIGN(sizeof(ch_packet_t),RTE_MBUF_PRIV_ALIGN),
												  pcontext->rx_mbuf_size, 
												  rte_socket_id());

    if(ppool->sambuf_pool == NULL){
        ch_log(CH_LOG_ERR,"create packet sa mbuf pool failed!");
       
		return NULL;
    }
#endif

   /*create all ports*/
   for(port_id = 0;port_id<port_n;port_id++){
   
      if(_port_enabled(port_id,port_mask)){
      
         port = _create_port(ppool,pcontext,port_id);
         if(port){
         
            /*add to pool*/
            ppool->port_n++;
            *(ch_port_t**)ch_array_push(ppool->ports) = port; 
         }
      }
   }

   return ppool;
}

static ch_port_t * _port_alloc(ch_port_pool_t *ppool){

   ch_port_t **ports;
   ch_port_t *port;

   if(ppool->cur_port_index>=ppool->port_n){
   
      ch_log(CH_LOG_ERR,"All port:%d has been alloced completely!",ppool->port_n);
      return NULL;
   }

   ports = (ch_port_t**)ppool->ports->elts;
   port = ports[ppool->cur_port_index];
   
   if(port->cur_port_queue_index<port->port_queue_n){
   
      return port;
   }else{
   
      if(ppool->cur_port_index+1>=ppool->port_n){
      
         ch_log(CH_LOG_ERR,"All port:%d has been alloced completely!",ppool->port_n);
         return NULL;
      } 
      port = ports[ppool->cur_port_index+1];
      ppool->cur_port_index++;
      return port;
   }
}

static ch_port_queue_t * _port_queue_alloc(ch_port_pool_t *ppool){

   ch_port_t *port;
   ch_port_queue_t *pq,**pqs;

   port = _port_alloc(ppool);
   if(port == NULL){
   
      return NULL;
   }

   pqs = (ch_port_queue_t**)port->port_queues->elts;
   pq = pqs[port->cur_port_queue_index];
   port->cur_port_queue_index++;

   return pq;
}

static int _port_queue_setup(ch_port_pool_t *ppool,ch_port_queue_t *pq,uint32_t socket){

    int ret;
	char pkmbuf_name[64] = {0};
	ch_port_context_t *pcontext = ppool->pcontext;

	_pool_name_init(ppool,pkmbuf_name,64);



	ret = rte_eth_rx_queue_setup(
			pq->port->port_id,
			pq->queue_id,
			(uint16_t)pcontext->port_nb_rxd,
			socket,
			&pq->port->rx_conf,
			ppool->pktmbuf_pool);

	if (ret < 0) {
		ch_log(CH_LOG_ERR,"Cannot setup RX queue %u for port %u (%d)",pq->queue_id,pq->port->port_id,ret);
		return -1;
	}

    /*ok*/
    return 0;
}

ch_port_queue_t * ch_port_pool_queue_bind(ch_port_pool_t *ppool,uint32_t socket){

   int ret;
   ch_port_queue_t *pq = NULL;

   pq = _port_queue_alloc(ppool);
   if(pq == NULL){
   
      ch_log(CH_LOG_ERR,"No port queue used to bind!");
      return NULL;
   }

   ret = _port_queue_setup(ppool,pq,socket);
   if(ret){
   
      ch_log(CH_LOG_ERR,"Cannot bind port:%d queue:%d,because setup failed!",pq->port->port_id,pq->queue_id);

      return NULL;
   }

   return pq;
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

    return port_up==1?0:-1;
}


static int _port_setup(ch_port_t *port){

	int ret;

	/* Start device */
	ret = rte_eth_dev_start(port->port_id);
	
	if (ret < 0){
	   ch_log(CH_LOG_ERR,"rte_eth_dev_start:err=%d, port=%u",
					  ret, (unsigned) port->port_id);
	   return -1;
	}

	rte_eth_promiscuous_enable(port->port_id);

    /*check the link status*/
    return check_port_link_status(port);

}

int ch_port_pool_setup(ch_port_pool_t *ppool){

   int i,n;
   ch_port_t *port,**ports;

   n = ppool->ports->nelts;
   ports = (ch_port_t**)ppool->ports->elts;
   
   for(i=0;i<n;i++){
   
      port = ports[i];
      if(_port_setup(port)){
      
         ch_log(CH_LOG_ERR,"Cannot setup port:%d",port->port_id);
         return -1;
      }
   }

   /* ok! */
   return 0;
}

void ch_port_pool_stat_dump(ch_port_t *port,FILE *fp){

	struct rte_eth_stats port_stat,*ps = &port_stat;

	if(rte_eth_stats_get(port->port_id,ps))
		return;

	fprintf(fp,"ps.ipackets%lu\n",(unsigned long)ps->ipackets);
	fprintf(fp,"ps.opackets%lu\n",(unsigned long)ps->opackets);
	fprintf(fp,"ps.ibytes%lu\n",(unsigned long)ps->ibytes);
	fprintf(fp,"ps.obytes%lu\n",(unsigned long)ps->obytes);
	fprintf(fp,"ps.imissed%lu\n",(unsigned long)ps->imissed);
	fprintf(fp,"ps.ierrors%lu\n",(unsigned long)ps->ierrors);
	fprintf(fp,"ps.oerrors%lu\n",(unsigned long)ps->oerrors);
	fprintf(fp,"ps.rx_nombuf%lu\n",(unsigned long)ps->rx_nombuf);
}
