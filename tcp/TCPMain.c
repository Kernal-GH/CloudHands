/*
 *
 *      Filename: TCPMain.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-06 17:24:16
 * Last Modified: 2018-04-04 17:23:47
 */

#include "ch_getopt.h"
#include "ch_log.h"
#include "ch_mpool.h"
#include "ch_signal.h"
#include "ch_tcp_work.h"

#define CMDLINE_OPTS "h"

static ch_tcp_work_t *tcp_work;

/**
 * Usage text.
 */
static void usage(void) {
    fprintf(stderr, "  Usage: TCPMain <rte args> [options] <path to the config file>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -h        This help\n\n");
}


static void tcp_exit(int rc){


	ch_tcp_work_stop(tcp_work);

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
            tcp_exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            tcp_exit(0);
        default:
            break;
    }


    printf("Caught unexpected signal %d\n", signum);
    tcp_exit(1);
}

int main(int argc,char **argv){

	int ret; 
    const char *cfname;
    ch_getopt_t *opt;
    ch_pool_t *mp;

    if(argc<2){
        usage();
        tcp_exit(-1);
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
        tcp_exit(-1);
    }

    /*process commandline opts*/
    ret = ch_getopt_init(&opt,mp,argc,argv);
    
    if(ret!=0){
        usage();
        tcp_exit(1);
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
                        tcp_exit(0);
                        break;
                     default:
                        usage();
                        tcp_exit(1);
                        break;
                }
                break;
            case CH_BADCH:
            case CH_BADARG:
                usage();
                tcp_exit(1);

            default:
                break;
        }
    } while (ret != CH_EOF);

    /*create context*/
    tcp_work = ch_tcp_work_create(mp,cfname);

    if(tcp_work == NULL){

        fprintf(stderr,"create context failed from config file%s\n",cfname);
        tcp_exit(1);
    }

    /*init log*/
    ch_log_init(mp,tcp_work->tcp_context->log_name,tcp_work->tcp_context->log_level);

	ch_signal(SIGINT, handle_signals);
	ch_signal(SIGTERM, handle_signals);

    return ch_tcp_work_start(tcp_work);
}
