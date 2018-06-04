/*
 * =====================================================================================
 *
 *       Filename:  Assemble.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/06/2017 03:25:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_getopt.h"
#include "ch_log.h"
#include "ch_mpool.h"
#include "ch_signal.h"
#include "ch_assemble_context.h"

#define CMDLINE_OPTS "h"

static ch_assemble_context_t *acontext;

/**
 * Usage text.
 */
static void usage(void) {
    fprintf(stderr, "  Usage: Assemble <rte args> [options] <path to the config file>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -h        This help\n\n");
}


static void assemble_exit(int rc){


	ch_assemble_context_stop(acontext);

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
            assemble_exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            assemble_exit(0);
        default:
            break;
    }


    printf("Caught unexpected signal %d\n", signum);
    assemble_exit(1);
}

int main(int argc,char **argv){

	int ret; 
    const char *cfname;
    ch_getopt_t *opt;
    ch_pool_t *mp;
	
    if(argc<2){
        usage();
        assemble_exit(-1);
    }
    
    /*argv last arg is config file path*/
    cfname = argv[argc-1];

    /* Init EAL */
	ret = rte_eal_init(argc, (char**)argv);
	if (ret < 0)
		return -1;
	argc -= ret;
	argv += ret;
    
    /*create global apr memory pool*/
    mp = ch_pool_create(4096);

    if(mp == NULL){

        fprintf(stderr,"create global memory pool failed!\n");
        assemble_exit(-1);
    }

    /*process commandline opts*/
    ret = ch_getopt_init(&opt,mp,argc,argv);
    
    if(ret!=0){
        usage();
        assemble_exit(1);
    }

    do {
        char  ch;
        const char *val;
        ret = ch_getopt(opt, CMDLINE_OPTS, &ch, &val);
        switch (ret) {
            case 0:
                switch (ch) {
                    case 'h':
                        usage();
                        assemble_exit(0);
                        break;
                     default:
                        usage();
                        assemble_exit(1);
                        break;
                }
                break;
            case CH_BADCH:
            case CH_BADARG:
                usage();
                assemble_exit(1);

            default:
                break;
        }
    } while (ret != CH_EOF);

    /*create context*/
    acontext = ch_assemble_context_create(mp,cfname);

    if(acontext == NULL){

        fprintf(stderr,"create context failed from config file%s\n",cfname);
        assemble_exit(1);
    }

    /*init log*/
    ch_log_init(mp,acontext->log_name,acontext->log_level);

	ch_signal(SIGINT, handle_signals);
	ch_signal(SIGTERM, handle_signals);

    return ch_assemble_context_start(acontext);
}

