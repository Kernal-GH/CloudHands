/*
 * =====================================================================================
 *
 *       Filename:  ch_port_context.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/02/2017 11:18:47 PM
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
#include "ch_config.h"
#include "ch_constants.h"

static void do_port_context_init(ch_port_context_t *pcontext){

	pcontext->rx_pthresh = RTE_PMD_PARAM_UNSET;
	pcontext->rx_hthresh = RTE_PMD_PARAM_UNSET;
	pcontext->rx_wthresh = RTE_PMD_PARAM_UNSET;

	pcontext->rx_free_thresh = RTE_PMD_PARAM_UNSET;
	pcontext->rx_drop_en = RTE_PMD_PARAM_UNSET;
	pcontext->rss_hf = ETH_RSS_IP;
    
	pcontext->port_max_pkt_burst = MAX_PKT_BURST_DEFAULT;
	pcontext->port_nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
	pcontext->port_n_rxq = N_RX_QUEUE_DEFAULT;
	pcontext->n_rx_mbuf = N_RX_MBUF_DEFAULT;
    
        pcontext->rx_mbuf_size = (9600 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM);
   
        pcontext->rx_mbuf_cache_size = 256;

	pcontext->jumbo_frame_max_size = 0x2600;
	pcontext->jumbo_frame =1;
}

static const char *cmd_rx_pthresh(cmd_parms * cmd ch_unused, void *_dcfg, const char *p1){


    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->rx_pthresh = (int8_t)atoi(p1);
    
    return NULL;
}

static const char *cmd_rx_hthresh(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->rx_hthresh = (int8_t)atoi(p1);
    
    return NULL;
}

static const char *cmd_rx_wthresh(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->rx_wthresh = (int8_t)atoi(p1);
    
    return NULL;
}

static const char *cmd_rx_free_thresh(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->rx_free_thresh = (int16_t)atoi(p1);
    
    return NULL;
}

static const char *cmd_rx_drop_enable(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->rx_drop_en = (int8_t)atoi(p1);
    
    return NULL;
}

static const char *cmd_rss_hf(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

	if(strcasecmp(RSS_HF_IP,p1) == 0){
		pcontext->rss_hf = ETH_RSS_IP;
	}else if(strcasecmp(RSS_HF_TCP,p1) == 0){
	
		pcontext->rss_hf = ETH_RSS_TCP;
	}else if(strcasecmp(RSS_HF_UDP,p1)==0){
		pcontext->rss_hf = ETH_RSS_UDP;
	
	}else if(strcasecmp(RSS_HF_SCTP,p1)==0){
	
		pcontext->rss_hf = ETH_RSS_SCTP;
	}else{
	
		return "unkown rss hf config item!";
	}

    return NULL;
}


static const char *cmd_max_pkt_burst(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->port_max_pkt_burst = (uint16_t)strtoul(p1,&endptr,10);
    
    if(pcontext->port_max_pkt_burst ==0){
        return "invalid port max pkt burst config value";
    }

    return NULL;
}


static const char *cmd_n_rx_mbuf(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->n_rx_mbuf = (uint32_t)strtoul(p1,&endptr,10);
    
    if(pcontext->n_rx_mbuf ==0){
        return "invalid r_rx_mbuf config value";
    }

    return NULL;
}

static const char *cmd_rx_mbuf_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->rx_mbuf_size = (uint32_t)strtoul(p1,&endptr,10);
    
    if(pcontext->rx_mbuf_size ==0){
        return "invalid rx mbuf size config value";
    }
    
    pcontext->rx_mbuf_size +=(sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM);

    return NULL;
}

static const char *cmd_n_rx_mbuf_cache_size(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->rx_mbuf_cache_size = (uint32_t)strtoul(p1,&endptr,10);
    

    return NULL;
}

static const char *cmd_port_nb_rxd(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->port_nb_rxd = (uint16_t)strtoul(p1,&endptr,10);
    
    if(pcontext->port_nb_rxd <=0){
        return "invalid port_nb_rxd config value";
    }

    return NULL;
}

static const char *cmd_port_n_rxq(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;

    pcontext->port_n_rxq = (uint16_t)strtoul(p1,&endptr,10);
    
    if(pcontext->port_n_rxq <=0){
        return "invalid port_n_rxq config value";
    }

    return NULL;
}

static const char *cmd_port_jumbo_frame(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_port_context_t *pcontext = (ch_port_context_t*)_dcfg;
    
    pcontext->jumbo_frame = strcasecmp(p1,"on")==0?1:0;
    
    pcontext->jumbo_frame_max_size = (uint32_t)strtoul(p2,&endptr,10);

    return NULL;
}


static const command_rec port_context_directives[] ={
    
    CH_INIT_TAKE1(
            "CHPortRXPThresh",
            cmd_rx_pthresh,
            NULL,
            0,
            "set values of RX prefetch threshold reg"
            ),

    CH_INIT_TAKE1(
            "CHPortRXHThresh",
            cmd_rx_hthresh,
            NULL,
            0,
            "set values of RX host threshold reg."
            ),

    CH_INIT_TAKE1(
            "CHPortRXWThresh",
            cmd_rx_wthresh,
            NULL,
            0,
            "set values of RX write-back threshold reg"
            ),

    CH_INIT_TAKE1(
            "CHPortRXFreeThresh",
            cmd_rx_free_thresh,
            NULL,
            0,
            "set value of RX free threshold"
            ),

    CH_INIT_TAKE1(
            "CHPortRXDropEnable",
            cmd_rx_drop_enable,
            NULL,
            0,
            "set value of RX drop enable"
            ),

    CH_INIT_TAKE1(
            "CHPortRSSHF",
            cmd_rss_hf,
            NULL,
            0,
            "set Receive Side Scaling (RSS) configuration"
            ),
    
	CH_INIT_TAKE1(
            "CHPortMaxPktBurst",
            cmd_max_pkt_burst,
            NULL,
            0,
            "set port max pkt burst"
            ),

    CH_INIT_TAKE1(
            "CHPortNBRxd",
            cmd_port_nb_rxd,
            NULL,
            0,
            "set number of RX ring descriptors "
            ),
    
    
    CH_INIT_TAKE1(
            "CHPortNRxq",
            cmd_port_n_rxq,
            NULL,
            0,
            "set number of rx queue per port"
            ),
    
    
    CH_INIT_TAKE1(
            "CHNRxMbuf",
            cmd_n_rx_mbuf,
            NULL,
            0,
            "set number of rx mbuf"
            ),
    
    CH_INIT_TAKE1(
            "CHNRxMbufSize",
            cmd_rx_mbuf_size,
            NULL,
            0,
            "set size of rx mbuf"
            ),
    
    CH_INIT_TAKE1(
            "CHNRxMbufCacheSize",
            cmd_n_rx_mbuf_cache_size,
            NULL,
            0,
            "set cache size of rx mbuf"
            ),
    
	CH_INIT_TAKE2(
            "CHPortJumboFrame",
            cmd_port_jumbo_frame,
            NULL,
            0,
            "set port jumbo frame"
            ),
};


ch_port_context_t * ch_port_context_create(ch_pool_t *mp,const char *cfname){

      const char *msg;
      ch_port_context_t *pcontext = NULL;
      
      if(cfname == NULL||strlen(cfname) == 0){
        ch_log(CH_LOG_ERR,"Must specify the config path port config!");
        return NULL;
      
      }

      pcontext = (ch_port_context_t*)ch_pcalloc(mp,sizeof(*pcontext));
      
      do_port_context_init(pcontext);
      msg = ch_process_command_config(port_context_directives,(void*)pcontext,mp,mp,cfname);

      /*config failed*/
      if(msg!=NULL){

        ch_log(CH_LOG_ERR,"Config port failed:%s",msg);
        return NULL;
    }
   return pcontext;
}
