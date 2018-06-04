/*
 * =====================================================================================
 *
 *       Filename:  ch_context.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月15日 16时35分20秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_config.h"
#include "ch_constants.h"
#include "ch_context.h"
#include "ch_log.h"

static void do_context_init(ch_context_t *context){


    context->cpool = NULL;
    context->port = NULL;
    context->rxtp = NULL;

    context->log_name = LOG_NAME_DEFAULT;
    context->log_level = LOG_LEVEL_DEFAULT;

    context->core_mask = 0;
    context->port_rx_mask = 0;

	context->n_rxtasks = 1;

    context->astask_ring_size = 0;

    context->tcp_cfname = "/usr/local/dpdk/CloudHands/conf/assemble_tcp.conf";
    context->udp_cfname = "/usr/local/dpdk/CloudHands/conf/assemble_udp.conf";

    context->app_tcp_cfname = "/usr/local/dpdk/CloudHands/conf/app_tcp.conf";
    context->app_udp_cfname = "/usr/local/dpdk/CloudHands/conf/app_udp.conf";

	context->port_cfname = "/usr/local/dpdk/CloudHands/conf/port.conf";
	context->packets_cfname = "/usr/local/dpdk/CloudHands/conf/packets.conf";
	context->crawl_cfname = "/usr/local/dpdk/CloudHands/conf/crawl.conf";
	
	context->debug_mode = 0;
	context->debug_mode_flush_count = 0;

	context->debug_rxpacket = 0;
}

static const char *cmd_log(cmd_parms *cmd, void *_dcfg, const char *p1,const char *p2){

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->log_name = p1;

    if(strcasecmp(p2,"info") == 0){
        context->log_level = CH_LOG_INFO;
    }else if(strcasecmp(p2,"debug") == 0){
        context->log_level = CH_LOG_DEBUG;
    }else if(strcasecmp(p2,"notice") == 0){
        context->log_level = CH_LOG_NOTICE;
    }else if(strcasecmp(p2,"warn") == 0){
        context->log_level = CH_LOG_WARN;
    }else if(strcasecmp(p2,"error") == 0){
        context->log_level = CH_LOG_ERR;
    }else if(strcasecmp(p2,"crit") == 0){
        context->log_level = CH_LOG_CRIT;
    }else if(strcasecmp(p2,"alert") == 0){
        context->log_level = CH_LOG_ALERT;
    }else if(strcasecmp(p2,"emerg") == 0){
        context->log_level = CH_LOG_EMERG;
    }else {

        return "unknown log level name!";
    }

    return NULL;
}

static const char *cmd_assemble_task_ring_size(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->astask_ring_size = strtoul(p1,&endptr,10);
    
    if(context->astask_ring_size <=0){
        return "invalid assemble task's ring size config value";
    }

    return NULL;
}


static const char *cmd_port_rx_mask(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_rx_mask = strtoul(p1,&endptr,16);
    
    return NULL;
}

static const char *cmd_core_mask(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->core_mask = strtoul(p1,&endptr,16);
    
    return NULL;
}

static const char *cmd_rxtask_n(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->n_rxtasks = strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_assemble_tcp_cfname(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->tcp_cfname = p1;
    
    return NULL;
}

static const char *cmd_assemble_udp_cfname(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->udp_cfname = p1;
    
    return NULL;
}

static const char *cmd_app_tcp_cfname(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->app_tcp_cfname = p1;
    
    return NULL;
}

static const char *cmd_app_udp_cfname(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->app_udp_cfname = p1;
    
    return NULL;
}

static const char *cmd_port_cfname(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_cfname = p1;
    
    return NULL;
}

static const char *cmd_packets_cfname(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->packets_cfname = p1;
    
    return NULL;
}

static const char *cmd_crawl_cfname(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->crawl_cfname = p1;
    
    return NULL;
}

static const char *cmd_debug_mode(cmd_parms *cmd, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

	context->debug_mode = strcasecmp(p1,"on")==0?1:0;
	
	context->debug_mode_flush_count = (size_t)strtoul(p2,&endptr,10);

    return NULL;
}

static const char *cmd_debug_rxp(cmd_parms *cmd, void *_dcfg, const char *p1){

    ch_context_t *context = (ch_context_t*)_dcfg;

	context->debug_rxpacket = strcasecmp(p1,"on")==0?1:0;
	
    return NULL;
}

static const command_rec context_directives[] ={
    
    CH_INIT_TAKE2(
            "CHLog",
            cmd_log,
            NULL,
            0,
            "set log name and level"
            ),
    
    
    CH_INIT_TAKE1(
            "CHCoreMask",
            cmd_core_mask,
            NULL,
            0,
            "set core mask"
            ),
    
    CH_INIT_TAKE1(
            "CHRXTasks",
            cmd_rxtask_n,
            NULL,
            0,
            "set the rcv packet tasks number"
            ),
    
	CH_INIT_TAKE1(
            "CHPortRxMask",
            cmd_port_rx_mask,
            NULL,
            0,
            "set port rx mask"
            ),
    
    CH_INIT_TAKE1(
            "CHAssembleTaskRingSize",
            cmd_assemble_task_ring_size,
            NULL,
            0,
            "set  assemble task's ring size"
            ),
    
    CH_INIT_TAKE1(
            "CHTCPAssembleCFName",
            cmd_assemble_tcp_cfname,
            NULL,
            0,
            "set assemble tcp config name"
            ),

    CH_INIT_TAKE1(
            "CHUDPAssembleCFName",
            cmd_assemble_udp_cfname,
            NULL,
            0,
            "set assemble udp config name"
            ),
    
	CH_INIT_TAKE1(
            "CHAppTCPCFName",
            cmd_app_tcp_cfname,
            NULL,
            0,
            "set app tcp config name"
            ),
    
	CH_INIT_TAKE1(
            "CHAppUDPCFName",
            cmd_app_udp_cfname,
            NULL,
            0,
            "set app udp config name"
            ),

	CH_INIT_TAKE1(
            "CHPortCFName",
            cmd_port_cfname,
            NULL,
            0,
            "set port config name"
            ),
	
	CH_INIT_TAKE1(
            "CHPacketsCFName",
            cmd_packets_cfname,
            NULL,
            0,
            "set packets interface config name"
            ),
	
	CH_INIT_TAKE1(
            "CHCrawlCFName",
            cmd_crawl_cfname,
            NULL,
            0,
            "set crawl ip/port list  config name"
            ),
	
	CH_INIT_TAKE2(
            "CHDebugMode",
            cmd_debug_mode,
            NULL,
            0,
            "set the debug mode"
            ),
	
	CH_INIT_TAKE1(
            "CHDebugRXPacket",
            cmd_debug_rxp,
            NULL,
            0,
            "set debug recieve packets"
            ),
};

ch_context_t * ch_context_create(apr_pool_t *mp,const char *cfname){

    const char *msg;

    ch_context_t *context = NULL;

    if(mp == NULL || cfname == NULL||strlen(cfname) == 0){
        return NULL;
    }

    context = (ch_context_t*)apr_pcalloc(mp,sizeof(ch_context_t));

    context->mp = mp;

    do_context_init(context);

    msg = ch_process_command_config(context_directives,(void*)context,context->mp,context->mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        printf("%s\n",msg);
        return NULL;
    }


    return context;
}

int ch_context_start(ch_context_t *context){

	/*create packets interface*/
	context->pin = ch_packets_interface_create(context->mp,context->packets_cfname);
	if(context->pin == NULL){
	
		ch_log(CH_LOG_ERR,"Create packets interface instance failed!");
		return CH_ERROR;
	}

	/*load crawl ip/port list */
	context->bwl = ch_black_white_list_load(context->mp,context->crawl_cfname);
	if(context->bwl == NULL){
	
		ch_log(CH_LOG_ERR,"Load crawl ip/port list failed!");
		return CH_ERROR;
	}

    /*create all cpu cores,*/
    context->cpool = ch_core_pool_create(context);
    if(context->cpool == NULL){
        ch_log(CH_LOG_ERR,"Create cpu cores for context failed!");
        return CH_ERROR;
    }

    /*create all ethernet ports*/
    context->port = ch_port_create(context,context->port_rx_mask);

    if(context->port == NULL){
        ch_log(CH_LOG_ERR,"Create ethernet ports for context failed!");
        return CH_ERROR;
    }

    /*create rxtask pool*/
    context->rxtp = ch_rxtask_pool_create(context);
    if(context->rxtp == NULL){
        ch_log(CH_LOG_ERR,"Create tasks for context failed!");
        return CH_ERROR;
    }

	/* setup port */
	if(CH_ERROR == ch_port_setup(context->port)){
	
		ch_log(CH_LOG_ERR,"Port setup failed!");
		return CH_ERROR;
	}

    /*setup all cores that were been bound to tasks*/
    ch_core_pool_cores_setup(context->cpool);

    /*ok*/
    return CH_OK;
}

void ch_context_stop(ch_context_t *context){

	if (context == NULL) return;

	ch_rxtask_pool_dump(context->rxtp);

}
