/*
 * =====================================================================================
 *
 *       Filename:  ch_pdispatcher_context.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/06/2017 03:22:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_constants.h"
#include "ch_log.h"
#include "ch_config.h"
#include "ch_pdispatcher_context.h"
#include "ch_packet.h"

static void do_pdcontext_init(ch_pdispatcher_context_t *pdcontext){

    pdcontext->cpool = NULL;
    pdcontext->ppool = NULL;
	pdcontext->rxtask_pool = NULL;
	pdcontext->pint_tcp_context = NULL;
	pdcontext->pint_sa_context = NULL;
	pdcontext->pint_udp_context = NULL;

    pdcontext->log_name = LOG_NAME_DEFAULT;
    pdcontext->log_level = LOG_LEVEL_DEFAULT;


    pdcontext->port_cfname = PORT_CFNAME_DEFAULT; 

	pdcontext->packet_rxtasks_n = 1;

	pdcontext->pint_tcp_cfname = PINT_TCP_CFNAME_DEFAULT;

	pdcontext->pint_sa_cfname = PINT_SA_CFNAME_DEFAULT;
	
	pdcontext->pint_udp_cfname = PINT_UDP_CFNAME_DEFAULT;

	pdcontext->ip_wlist_mmap_fname = NULL;
	pdcontext->ip_wlist_msize = 65536;
	
	pdcontext->ip_blist_mmap_fname = NULL;
	pdcontext->ip_blist_msize = 65536;
	
    pdcontext->stat_mmap_fname = "/opt/data/cloudhands/store/sa_stat.data";
	pdcontext->stat_time_up = 7*24*3600;
	pdcontext->stat_time_tv = 5*60;

    pdcontext->redis_ipwblist_fname = NULL;
    pdcontext->ip_wblist = NULL;

}

static const char *cmd_log(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    ch_pdispatcher_context_t  *pdcontext = (ch_pdispatcher_context_t *)_dcfg;

    pdcontext->log_name = p1;

    if(strcasecmp(p2,"info") == 0){
        pdcontext->log_level = CH_LOG_INFO;
    }else if(strcasecmp(p2,"debug") == 0){
        pdcontext->log_level = CH_LOG_DEBUG;
    }else if(strcasecmp(p2,"notice") == 0){
        pdcontext->log_level = CH_LOG_NOTICE;
    }else if(strcasecmp(p2,"warn") == 0){
        pdcontext->log_level = CH_LOG_WARN;
    }else if(strcasecmp(p2,"error") == 0){
        pdcontext->log_level = CH_LOG_ERR;
    }else if(strcasecmp(p2,"crit") == 0){
        pdcontext->log_level = CH_LOG_CRIT;
    }else if(strcasecmp(p2,"alert") == 0){
        pdcontext->log_level = CH_LOG_ALERT;
    }else if(strcasecmp(p2,"emerg") == 0){
        pdcontext->log_level = CH_LOG_EMERG;
    }else {

        return "unknown log level name!";
    }

    return NULL;
}

static const char *cmd_port_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;

    pdcontext->port_cfname  = p1;
    
    return NULL;
}

static const char *cmd_port_rx_mask(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;

    pdcontext->port_mask = strtoul(p1,&endptr,16);
    
    return NULL;
}

static const char *cmd_packet_rxtasks(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;

    pdcontext->packet_rxtasks_n = strtoul(p1,&endptr,10);

	if(pdcontext->packet_rxtasks_n<1){
	
		return "The number of packet receive tasks must gt:1!";
	}

    return NULL;
}


static const char *cmd_pint_tcp_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;

    pdcontext->pint_tcp_cfname = p1;

    return NULL;
}

static const char *cmd_pint_udp_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;

    pdcontext->pint_udp_cfname = p1;

    return NULL;
}

static const char *cmd_pint_sa_cfname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){


    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;

    pdcontext->pint_sa_cfname = p1;

    return NULL;
}

static const char *cmd_ip_white_list(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;
    
	pdcontext->ip_wlist_mmap_fname = p1;

	pdcontext->ip_wlist_msize = (size_t)strtoul(p2,&endptr,10);


    return NULL;
}

static const char *cmd_redis_ipwblist_fname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;
    
	pdcontext->redis_ipwblist_fname = p1;


    return NULL;
}

static const char *cmd_ip_black_list(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1,const char *p2){

    char *endptr;

    ch_pdispatcher_context_t *pdcontext = (ch_pdispatcher_context_t*)_dcfg;
    
	pdcontext->ip_blist_mmap_fname = p1;

	pdcontext->ip_blist_msize = (size_t)strtoul(p2,&endptr,10);


    return NULL;
}

static const char *cmd_stat_mmap_fname(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    ch_pdispatcher_context_t *context = (ch_pdispatcher_context_t*)_dcfg;

	context->stat_mmap_fname = p1;

    return NULL;
}

static const char *cmd_stat_timeup(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pdispatcher_context_t *context = (ch_pdispatcher_context_t*)_dcfg;

    context->stat_time_up = (uint64_t)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const char *cmd_stat_timetv(cmd_parms *cmd ch_unused, void *_dcfg, const char *p1){

    char *endptr;

    ch_pdispatcher_context_t *context = (ch_pdispatcher_context_t*)_dcfg;

    context->stat_time_tv = (uint64_t)strtoul(p1,&endptr,10);
    
    return NULL;
}

static const command_rec pdcontext_directives[] ={
    
    CH_INIT_TAKE2(
            "CHLog",
            cmd_log,
            NULL,
            0,
            "set log name and level"
            ),
     
    CH_INIT_TAKE1(
            "CHPortCFName",
            cmd_port_cfname,
            NULL,
            0,
            "set the port config file path"
            ),
	
    CH_INIT_TAKE1(
            "CHRXTasks",
            cmd_packet_rxtasks,
            NULL,
            0,
            "set packet receive task's number"
            ),

	CH_INIT_TAKE1(
            "CHPortRxMask",
            cmd_port_rx_mask,
            NULL,
            0,
            "set port rx mask"
            ),
	
	CH_INIT_TAKE1(
            "CHTCPPintCFName",
            cmd_pint_tcp_cfname,
            NULL,
            0,
            "set process interface tcp context  config file path"
            ),

	CH_INIT_TAKE1(
            "CHSAPintCFName",
            cmd_pint_sa_cfname,
            NULL,
            0,
            "set process interface sa context  config file path"
            ),

	CH_INIT_TAKE1(
            "CHUDPPintCFName",
            cmd_pint_udp_cfname,
            NULL,
            0,
            "set process interface udp context  config file path"
            ),

    CH_INIT_TAKE2(
            "CHIPWList",
            cmd_ip_white_list,
            NULL,
            0,
            "set ip white list path and size"
            ),

    CH_INIT_TAKE2(
            "CHIPBList",
            cmd_ip_black_list,
            NULL,
            0,
            "set ip black list path and size"
            ),
    
    CH_INIT_TAKE1(
            "CHRedisIPWBListFName",
            cmd_redis_ipwblist_fname,
            NULL,
            0,
            "set redis ip wblist config file path"
            ),
	
    CH_INIT_TAKE1(
            "CHStatMMapFName",
            cmd_stat_mmap_fname,
            NULL,
            0,
            "set the statistic mmap file name"
            ),
	
	CH_INIT_TAKE1(
            "CHStatTimeUP",
            cmd_stat_timeup,
            NULL,
            0,
            "set the statistic time up"
            ),

	CH_INIT_TAKE1(
            "CHStatTimeTV",
            cmd_stat_timetv,
            NULL,
            0,
            "set the statistic time tv"
            ),
};

ch_pdispatcher_context_t * ch_pdispatcher_context_create(ch_pool_t *mp,const char *cfname){


    ch_pdispatcher_context_t * pdcontext = NULL;

	const char *msg;
	
    if(mp == NULL || cfname == NULL||strlen(cfname) == 0){
        return NULL;
    }

    pdcontext = (ch_pdispatcher_context_t*)ch_pcalloc(mp,sizeof(*pdcontext));

    pdcontext->mp = mp;

    do_pdcontext_init(pdcontext);

    msg = ch_process_command_config(pdcontext_directives,(void*)pdcontext,mp,mp,cfname);

    /*config failed*/
    if(msg!=NULL){

        printf("%s\n",msg);
        return NULL;
    }

    return pdcontext;
}

static int _check_cores_ports(ch_core_pool_t *cpool,ch_port_pool_t *ppool){

	if(cpool->config_core_count<=0){
	
		ch_log(CH_LOG_ERR,"no cpu core configged to receive packets!");
		return -1;
	}
	
	if(ppool->port_n<=0||ppool->port_queue_n<=0){
	
		ch_log(CH_LOG_ERR,"no port configged to receive packets!");
		return -1;
	}

#if 0
	if(cpool->config_core_count<ppool->port_queue_n){
	
		ch_log(CH_LOG_ERR,
			"The port queue number:%d is too long,no more cpu cores:%d used to receive packets from these ports!",
			ppool->port_queue_n,cpool->config_core_count);
		return -1;
	}
#endif
	/*ok!*/
	return 0;
}



int ch_pdispatcher_context_start(ch_pdispatcher_context_t *pdcontext){

	if(ch_wb_list_ip_init(&pdcontext->ip_white_list,pdcontext->ip_wlist_mmap_fname,pdcontext->ip_wlist_msize))
	{
	
		ch_log(CH_LOG_ERR,"Cannot load ip white list!");
		return -1;
	}
	
	if(ch_wb_list_ip_init(&pdcontext->ip_black_list,pdcontext->ip_blist_mmap_fname,pdcontext->ip_blist_msize))
	{
	
		ch_log(CH_LOG_ERR,"Cannot load ip black list!");
		return -1;
	}

    pdcontext->ip_wblist = ch_redis_ip_wblist_create(pdcontext->mp,pdcontext->redis_ipwblist_fname);
    if(pdcontext->ip_wblist == NULL){

        ch_log(CH_LOG_ERR,"Cannot load redis ip wblist config!");
        return -1;
    }

	pdcontext->pint_tcp_context = ch_process_interface_tcp_context_create(pdcontext->mp,
		pdcontext->pint_tcp_cfname,1); 

	if(pdcontext->pint_tcp_context == NULL){
	
		ch_log(CH_LOG_ERR,"Create process tcp interface context failed!");
		return -1;
	}
	
	pdcontext->pint_tcp_context->ip_white_list = &pdcontext->ip_white_list;
	pdcontext->pint_tcp_context->ip_black_list = &pdcontext->ip_black_list;

	pdcontext->pint_sa_context = ch_process_interface_sa_context_create(pdcontext->mp,
		pdcontext->pint_sa_cfname,1); 

	if(pdcontext->pint_sa_context == NULL){
	
		ch_log(CH_LOG_ERR,"Create process sa interface context failed!");
		return -1;
	}
	
	pdcontext->pint_udp_context = ch_process_interface_udp_context_create(pdcontext->mp,
		pdcontext->pint_udp_cfname,1); 

	if(pdcontext->pint_udp_context == NULL){
	
		ch_log(CH_LOG_ERR,"Create process udp interface context failed!");
		return -1;
	}
	
	pdcontext->pint_udp_context->ip_white_list = &pdcontext->ip_white_list;
	pdcontext->pint_udp_context->ip_black_list = &pdcontext->ip_black_list;

	/*create cpu core pool*/
	pdcontext->cpool = ch_core_pool_create(pdcontext->mp,NULL);
	if(pdcontext->cpool == NULL){
	
		ch_log(CH_LOG_ERR,"Create cpu core pool failed!");
		return -1;
	}

	/*create packet recieve port pool*/
	pdcontext->ppool = ch_port_pool_create(pdcontext->mp,pdcontext->port_cfname,
		PKT_POOL_NAME,
		SA_POOL_NAME,
		pdcontext->port_mask);

	if(pdcontext->ppool == NULL){

		ch_log(CH_LOG_ERR,"Create packet receive port pool failed!");
		return -1;
	}

	if(_check_cores_ports(pdcontext->cpool,pdcontext->ppool)){

		/*failed*/
		ch_log(CH_LOG_ERR,"cpu cores and packet receive ports config is invalid,please reconfig them!");
		return -1;
	}

	/*init packet*/
	ch_packet_init();
	
    pdcontext->st_pool = ch_stat_pool_create(pdcontext->mp,pdcontext->stat_mmap_fname,
		pdcontext->stat_time_up,pdcontext->stat_time_tv);

	if(pdcontext->st_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create statistic pool!");
		return -1;
	}

	/*Create packet receive task pool*/
	pdcontext->rxtask_pool = ch_packet_rxtask_pool_create(pdcontext);

	if(pdcontext->rxtask_pool == NULL){
	
		ch_log(CH_LOG_ERR,"create packet receive task pool failed!");
		return -1;
	}

	/*start all ports*/
	if(ch_port_pool_setup(pdcontext->ppool)){
	
		ch_log(CH_LOG_ERR,"setup ports pool failed!");
		return -1;
	}

	/*start all cpu cores to start receive packets from ports*/
	if(ch_core_pool_slaves_setup(pdcontext->cpool)){
	
		ch_log(CH_LOG_ERR,"setup all slave threads failed!");
		return -1;
	}

	ch_core_pool_wait_for_slaves(pdcontext->cpool);
	/*ok!*/

	return 0;
}

void ch_pdispatcher_context_stop(ch_pdispatcher_context_t *pdcontext ch_unused){


}

