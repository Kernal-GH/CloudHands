/*
 *
 *      Filename: ch_proto_main.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 02:22:07
 * Last Modified: 2016-10-28 04:21:47
 */

#include <stdio.h>
#include <stdlib.h>
#include <apr_getopt.h>
#include <apr_signal.h>
#include "ch_errno.h"
#include "ch_log.h"
#include "ch_proto_context.h"
#include "ch_constants.h"

#define CMDLINE_OPTS "h"

static ch_proto_context_t *g_pcontext;

/**
 * Usage text.
 */
static void usage(void) {
    fprintf(stderr, "  Usage: parse_proto [options] <path to the config file> <mmap file path>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -h        This help\n\n");
}


static void parse_proto_exit(int rc){

	/* unused */
	rc = rc;

    ch_proto_context_exit(g_pcontext);
}

/**
 * Handle signals.
 */
static void handle_signals(int signum)
{
    switch (signum) {
        case SIGINT:
            printf("Caught SIGINT, shutting down.\n");
            parse_proto_exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            parse_proto_exit(0);
        default:
            break;
    }

    printf("Caught unexpected signal %d\n", signum);
    parse_proto_exit(1);
}

int main(int argc,const char * const argv[]){
    
    const char *cfname;
    const char *mfname;

    apr_status_t rc;
    apr_getopt_t *opt;
    apr_pool_t *mp;
    ch_proto_context_t  *pcontext;
    
    if(argc<3){
        usage();
    
        return -1;
    }
    
    mfname = argv[argc-1];
    cfname = argv[argc-2];

    /*init apr app runtine*/
    apr_app_initialize(&argc,&argv,NULL);

    /*create global apr memory pool*/
    apr_pool_create(&mp,NULL);

    if(mp == NULL){

        fprintf(stderr,"create global memory pool failed!\n");
        return -1;
    }

    /*process commandline opts*/
    rc = apr_getopt_init(&opt,mp,argc,argv);
    
    if(rc!=APR_SUCCESS){
        usage();
    
        return -1;
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
                        return 0;
                     default:
                        usage();
                        return -1;
                }
                break;
            case APR_BADCH:
            case APR_BADARG:
                usage();
                return -1;

            default:
                break;
        }
    } while (rc != APR_EOF);

    /*create context*/
    pcontext = ch_proto_context_create(mp,cfname,mfname);

    if(pcontext == NULL){

        fprintf(stderr,"create proto context failed from config file%s\n",cfname);
        return -1;
    }

    /*init errno*/
    rc = ch_strerror_init();
    if(rc!=CH_OK){
        fprintf(stderr,"init errno failed!\n");
        return -1;
    }
    
    /*init log*/
    ch_log_init(pcontext->mp,pcontext->log_file,pcontext->log_level);

	apr_signal(SIGINT, handle_signals);
	apr_signal(SIGTERM, handle_signals);

    g_pcontext = pcontext;

    /* start context */
    if(ch_proto_context_start(pcontext)){
    
        fprintf(stderr,"start proto context failed!\n");
        return -1;
    }

	return 0;
}


