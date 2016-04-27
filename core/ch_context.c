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


    context->app_context = NULL;
    context->cpool = NULL;
    context->ppool = NULL;
    context->tpool = NULL;

    context->log_name = LOG_NAME_DEFAULT;
    context->log_level = LOG_LEVEL_DEFAULT;

    context->port_max_pkt_burst = MAX_PKT_BURST_DEFAULT;
    context->port_nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
    context->port_nb_txd = RTE_TEST_TX_DESC_DEFAULT;
    context->port_n_rxq = N_RX_QUEUE_DEFAULT;
    context->port_n_txq = N_TX_QUEUE_DEFAULT;

    context->n_rx_mbuf = N_RX_MBUF_DEFAULT;

    context->core_mask = 0;
    context->port_mask = 0;
    context->port_rx_mask = 0;

    context->astask_ring_size = 0;
    context->n_assemble_tasks = 0;

    context->n_session_requests_limit = 0;
    context->n_assemble_sessions_limit = 0;
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

static const char *cmd_max_pkt_burst(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_max_pkt_burst = strtoul(p1,&endptr,10);
    
    if(context->port_max_pkt_burst <=0){
        return "invalid port max pkt burst config value";
    }

    return NULL;
}


static const char *cmd_n_rx_mbuf(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->n_rx_mbuf = strtoul(p1,&endptr,10);
    
    if(context->n_rx_mbuf <=0){
        return "invalid r_rx_mbuf config value";
    }

    return NULL;
}

static const char *cmd_port_nb_rxd(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_nb_rxd = strtoul(p1,&endptr,10);
    
    if(context->port_nb_rxd <=0){
        return "invalid port_nb_rxd config value";
    }

    return NULL;
}

static const char *cmd_port_nb_txd(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_nb_txd = strtoul(p1,&endptr,10);
    
    if(context->port_nb_txd <=0){
        return "invalid port_nb_txd config value";
    }

    return NULL;
}

static const char *cmd_port_n_rxq(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_n_rxq = strtoul(p1,&endptr,10);
    
    if(context->port_n_rxq <=0){
        return "invalid port_n_rxq config value";
    }

    return NULL;
}

static const char *cmd_port_n_txq(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_n_txq = strtoul(p1,&endptr,10);
    
    if(context->port_n_txq <=0){
        return "invalid port_n_txq config value";
    }

    return NULL;
}

static const char *cmd_n_assemble_tasks(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->n_assemble_tasks = strtoul(p1,&endptr,10);
    
    if(context->n_assemble_tasks <=0){
        return "invalid assemble tasks number config value";
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

static const char *cmd_n_session_requests_limit(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->n_session_requests_limit = strtoul(p1,&endptr,10);
    

    return NULL;
}

static const char *cmd_n_assemble_sessions_limit(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->n_assemble_sessions_limit = strtoul(p1,&endptr,10);
    

    return NULL;
}

static const char *cmd_port_mask(cmd_parms *cmd, void *_dcfg, const char *p1){

    char *endptr;

    ch_context_t *context = (ch_context_t*)_dcfg;

    context->port_mask = strtoul(p1,&endptr,16);
    
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

static const command_rec context_directives[] ={
    
    CH_INIT_TAKE2(
            "CHLog",
            cmd_log,
            NULL,
            0,
            "set log name and level"
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
            "CHPortNBTxd",
            cmd_port_nb_txd,
            NULL,
            0,
            "set number of TX ring descriptors"
            ),
    
    CH_INIT_TAKE1(
            "CHPortNRxq",
            cmd_port_n_rxq,
            NULL,
            0,
            "set number of rx queue per port"
            ),
    
    CH_INIT_TAKE1(
            "CHPortNTxq",
            cmd_port_n_txq,
            NULL,
            0,
            "set number of tx queue per port"
            ),
    
    CH_INIT_TAKE1(
            "CHNRxMbuf",
            cmd_n_rx_mbuf,
            NULL,
            0,
            "set number of rx mbuf"
            ),
    
    CH_INIT_TAKE1(
            "CHCoreMask",
            cmd_core_mask,
            NULL,
            0,
            "set core mask"
            ),
    
    CH_INIT_TAKE1(
            "CHPortMask",
            cmd_port_mask,
            NULL,
            0,
            "set port mask"
            ),
    
    CH_INIT_TAKE1(
            "CHPortRxMask",
            cmd_port_rx_mask,
            NULL,
            0,
            "set port rx mask"
            ),
    
    CH_INIT_TAKE1(
            "CHAssembleTasksN",
            cmd_n_assemble_tasks,
            NULL,
            0,
            "set number of assemble tasks"
            ),
    
    CH_INIT_TAKE1(
            "CHAssembleTaskRingSize",
            cmd_assemble_task_ring_size,
            NULL,
            0,
            "set  assemble task's ring size"
            ),
    CH_INIT_TAKE1(
            "CHNSessionRequestsLimit",
            cmd_n_session_requests_limit,
            NULL,
            0,
            "set number of session requests limit"
            ),
    
    CH_INIT_TAKE1(
            "CHNAssembleSessionsLimit",
            cmd_n_assemble_sessions_limit,
            NULL,
            0,
            "set number of assemble sessions limit"
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

    /*create all cpu cores,*/
    context->cpool = ch_core_pool_create(context);
    if(context->cpool == NULL){
        ch_log(CH_LOG_ERR,"Create cpu cores for context failed!");
        return CH_ERROR;
    }

    /*create all ethernet ports*/
    context->ppool = ch_port_pool_create(context);
    if(context->ppool == NULL){
        ch_log(CH_LOG_ERR,"Create ethernet ports for context failed!");
        return CH_ERROR;
    }

    /*create all tasks*/
    context->tpool = ch_task_pool_create(context);
    if(context->tpool == NULL){
        ch_log(CH_LOG_ERR,"Create tasks for context failed!");
        return CH_ERROR;
    }

    /*create application context*/
    context->app_context = ch_app_context_create(context);
    if(context->app_context == NULL){
        ch_log(CH_LOG_ERR,"Create application context for context failed!");
        return CH_ERROR;
    }

    ch_http_init(context->app_context);
    
    /*setup all cores that were been bound to tasks*/
    ch_core_pool_cores_setup(context->cpool);

    /*ok*/
    return CH_OK;
}

void ch_context_stop(ch_context_t *context){

}
