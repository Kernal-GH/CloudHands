/*
 * =====================================================================================
 *
 *       Filename:  cloudhands.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月14日 13时51分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <rte_eal.h>
#include <apr_getopt.h>
#include <apr_signal.h>
#include "ch_errno.h"
#include "ch_log.h"
#include "ch_context.h"
#include "ch_constants.h"
#include "ch_task.h"

#define CMDLINE_OPTS "h"

/**
 * Usage text.
 */
static void usage(void) {
    fprintf(stderr, "  Usage: cloudhands <rte args> [options] <path to the config file>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -h        This help\n\n");
}


static void cloudhands_exit(int rc){

    exit(rc);
}

/**
 * Handle signals.
 */
static void handle_signals(int signum)
{
    switch (signum) {
        case SIGINT:
            printf("Caught SIGINT, shutting down.\n");
            cloudhands_exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            cloudhands_exit(0);
        default:
            break;
    }


    printf("Caught unexpected signal %d\n", signum);
    cloudhands_exit(1);
}

static void run_task(ch_context_t *context,unsigned int core_id){

    ch_task_t **tasks;
    ch_task_t *task;
    int i;

    tasks = (ch_task_t**)context->tasks->elts;

    for(i=0;i<context->tasks->nelts;i++){

        task = tasks[i];
        if(task->core->core_id == core_id){

            ch_log(CH_LOG_INFO,"start task in cpu core[%d]",core_id);
            ch_task_init(task,NULL);
            ch_task_run(task,NULL);
        }
    }

    ch_log(CH_LOG_WARN,"no task in cpu core[%d]",core_id);
}

static int main_run_task(void *user_data){

    ch_context_t *context = (ch_context_t *)user_data;

    unsigned int core_id = rte_lcore_id();

    run_task(context,core_id);
}

int main(int argc,const char * const argv[]){
    
	int ret;
    
    const char *cfname;
    apr_status_t rc;
    apr_getopt_t *opt;
    apr_pool_t *mp;
    ch_context_t *context;

    ch_task_t **tasks;
    ch_task_t *task;
    uint32_t core_n;
    uint32_t port_n;
    uint32_t core_id;
    int i;

    if(argc<2){
        usage();
        cloudhands_exit(-1);
    }
    
    /*argv last arg is config file path*/
    cfname = argv[argc-1];

    /* Init EAL */
	ret = rte_eal_init(argc, (char**)argv);
	if (ret < 0)
		return -1;
	argc -= ret;
	argv += ret;
    
    /*init apr app runtine*/
    apr_app_initialize(&argc,&argv,NULL);

    /*create global apr memory pool*/
    apr_pool_create(&mp,NULL);

    if(mp == NULL){

        fprintf(stderr,"create global memory pool failed!\n");
        cloudhands_exit(-1);
    }

    /*process commandline opts*/
    rc = apr_getopt_init(&opt,mp,argc,argv);
    
    if(rc!=APR_SUCCESS){
        usage();
        cloudhands_exit(1);
    }

    do {
        char  ch;
        const char *val;
        rc = apr_getopt(opt, CMDLINE_OPTS, &ch, &val);
        switch (rc) {
            case APR_SUCCESS:
                switch (ch) {
                    case 'h':
                        usage();
                        cloudhands_exit(0);
                        break;
                     default:
                        usage();
                        cloudhands_exit(1);
                        break;
                }
                break;
            case APR_BADCH:
            case APR_BADARG:
                usage();
                cloudhands_exit(1);

            default:
                break;
        }
    } while (rc != APR_EOF);

    /*create context*/
    context = ch_context_create(mp,cfname);

    if(context == NULL){

        fprintf(stderr,"create context failed from config file%s\n",cfname);
        cloudhands_exit(1);
    }

    /*init errno*/
    rc = ch_strerror_init();
    if(rc!=CH_OK){
        fprintf(stderr,"init errno failed!\n");
        cloudhands_exit(1);
    }
    
    /*init log*/
    ch_log_init(context->mp,context->log_name,context->log_level);

	apr_signal(SIGINT, handle_signals);
	apr_signal(SIGTERM, handle_signals);

    /*init cpu cores*/
    core_n = rte_lcore_count();
    if(core_n == 0){

        ch_log(CH_LOG_ERR,"no cpu core specified for cloudhands!");
        cloudhands_exit(1);
    }

    ret = ch_context_core_init(context,core_n);

    if(ret == -1){
        ch_log(CH_LOG_ERR,"create cpu core failed for cloudhands!");
        cloudhands_exit(1);
    }
    
    ch_log(CH_LOG_INFO,"create cpu cores[%d] for cloudhands!",ret);

    /*init network ports*/
    port_n = rte_eth_dev_count();
    if(port_n == 0){

        ch_log(CH_LOG_ERR,"no network port specified for cloudhands!");
        cloudhands_exit(1);
    }

    ret = ch_context_port_init(context,port_n);

    if(ret == -1){

        ch_log(CH_LOG_ERR,"create network ports failed!\n");
        cloudhands_exit(1);
    }

    ch_log(CH_LOG_INFO,"create network ports[%d] for cloudhands!",ret);

    /*init receive packat network ports*/
    ret = ch_context_rxport_init(context);
    if(ret == -1){
        ch_log(CH_LOG_ERR,"init receive packete network ports failed!");
        cloudhands_exit(1);
    }

    ret = ch_context_app_init(context);
    if(ret == -1){
        ch_log(CH_LOG_ERR,"init application context failed!");
        cloudhands_exit(1);
    }

   /* launch per-lcore init on every lcore */
    rte_eal_mp_remote_launch(main_run_task, (void*)context, CALL_MASTER);

    RTE_LCORE_FOREACH_SLAVE(core_id) {
        if (rte_eal_wait_lcore(core_id) < 0){
            ch_log(CH_LOG_ERR,"wait all tasks failed!");
            cloudhands_exit(1);        
        }
    } 

    return 0;
}
