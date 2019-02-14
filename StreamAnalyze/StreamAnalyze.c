/*
 *
 *      Filename: StreamAnalyze.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-21 11:40:55
 * Last Modified: 2018-04-09 15:38:10
 */

#include "ch_getopt.h"
#include "ch_log.h"
#include "ch_mpool.h"
#include "ch_signal.h"
#include "ch_sa_work.h"

#define CMDLINE_OPTS "h"

static ch_sa_work_t *sa_work;

/**
 * Usage text.
 */
static void usage(void) {
    fprintf(stderr, "  Usage: TCPMain <rte args> [options] <path to the config file>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -h        This help\n\n");
}


static void sa_exit(int rc){


	ch_sa_work_stop(sa_work);

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
            sa_exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            sa_exit(0);
        default:
            break;
    }


    printf("Caught unexpected signal %d\n", signum);
    sa_exit(1);
}

int main(int argc,char **argv){

	int ret; 
    const char *cfname;
    ch_getopt_t *opt;
    ch_pool_t *mp;

    if(argc<2){
        usage();
        sa_exit(-1);
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
        sa_exit(-1);
    }

    /*process commandline opts*/
    ret = ch_getopt_init(&opt,mp,argc,argv);
    
    if(ret!=0){
        usage();
        sa_exit(1);
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
                        sa_exit(0);
                        break;
                     default:
                        usage();
                        sa_exit(1);
                        break;
                }
                break;
            case CH_BADCH:
            case CH_BADARG:
                usage();
                sa_exit(1);

            default:
                break;
        }
    } while (ret != CH_EOF);

    /*create context*/
    sa_work = ch_sa_work_create(mp,cfname);

    if(sa_work == NULL){

        fprintf(stderr,"create context failed from config file%s\n",cfname);
        sa_exit(1);
    }



	ch_signal(SIGINT, handle_signals);
	ch_signal(SIGTERM, handle_signals);

    return ch_sa_work_start(sa_work);
}
