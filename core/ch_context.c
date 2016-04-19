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
#include "ch_core.h"
#include "ch_port.h"
#include "ch_rxtask.h"

static void do_context_init(ch_context_t *context){


    context->app_context = NULL;
    context->cores = apr_array_make(context->mp,16,sizeof(ch_core_t*));
    context->ports = apr_array_make(context->mp,16,sizeof(ch_port_t*));
    context->tasks = apr_array_make(context->mp,16,sizeof(ch_task_t*));

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

ch_core_t * ch_context_core_pop(ch_context_t *context){

    ch_core_t *core = NULL;

    if(context->cores->nelts == 0)
        return NULL;

    core = (ch_core_t*)apr_array_pop(context->cores);

}

void ch_context_core_push(ch_context_t *context,ch_core_t *core){

    *(ch_core_t **)apr_array_push(context->cores) = core;
}

void ch_context_port_push(ch_context_t *context,ch_port_t *port){

    *(ch_port_t **)apr_array_push(context->ports) = port;
}

void ch_context_task_push(ch_context_t *context,ch_task_t *task){

    *(ch_task_t**)apr_array_push(context->tasks) = task;
}

int ch_context_core_init(ch_context_t *context,uint32_t core_n){

    ch_core_t *core;
    unsigned int lcore_id;

    for(lcore_id = 0; lcore_id <core_n;lcore_id++){

        if(rte_lcore_is_enabled(lcore_id)==0){
            continue;
        }

        printf("create core[%d]\n",lcore_id);

        core = ch_core_create(context->mp,lcore_id);

        if(core == NULL){
            printf("create core[%d] failed!\n",lcore_id);
            return -1;
        }

        ch_context_core_push(context,core);
    }

    return lcore_id;
}

int ch_context_port_init(ch_context_t *context,uint32_t port_n){

    ch_port_t *port;
    unsigned int port_id;
    
    for(port_id = 0; port_id < port_n; port_id++){

        /*skip ports that are not enabled*/
        if((context->port_mask &(1 << port_id)) == 0){

            printf("Skipping disabled port %d\n",port_id);
            continue;
        }

        printf("create port[%d]\n",port_id);

        port = ch_port_create(context,port_id);

        if(port == NULL){

            printf("create port[%d] failed!\n",port_id);
            return -1;
        }

        ch_context_port_push(context,port);
   } 

    return port_id;
}

int ch_context_rxport_init(ch_context_t *context){

    ch_task_t *task = ch_rxtask_create(context,context->port_rx_mask);

    if(task == NULL){

        printf("create rxtask failed!\n");
        return -1;
    }

    ch_context_task_push(context,task);

    return 0;
}

int ch_context_app_init(ch_context_t *context){

    ch_app_context_t *app_context = ch_app_context_create(context);

    if(app_context == NULL){
        
        printf("Create application context failed!\n");
        return -1;
    }

    context->app_context = app_context;

    return 0;
}
