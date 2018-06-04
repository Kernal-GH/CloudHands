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

#define CMDLINE_OPTS "h"

static ch_context_t *context;

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


	ch_context_stop(context);

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

int main(int argc,const char * const argv[]){
    
	int ret;
    
    const char *cfname;
    apr_status_t rc;
    apr_getopt_t *opt;
    apr_pool_t *mp;
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

    return ch_context_start(context);

}
