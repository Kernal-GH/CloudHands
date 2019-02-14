/*
 *
 *      Filename: UDPMain.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-25 10:53:34
 * Last Modified: 2018-05-11 15:40:02
 */
#include "ch_getopt.h"
#include "ch_log.h"
#include "ch_mpool.h"
#include "ch_signal.h"
#include "ch_udp_work.h"

#define CMDLINE_OPTS "h"

static ch_udp_work_t *udp_work;

/**
 * Uudpge text.
 */
static void usage(void) {
    fprintf(stderr, "  Usage: TCPMain <rte args> [options] <path to the config file>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -h        This help\n\n");
}


static void udp_exit(int rc){


	ch_udp_work_stop(udp_work);

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
            udp_exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            udp_exit(0);
        default:
            break;
    }


    printf("Caught unexpected signal %d\n", signum);
    udp_exit(1);
}

int main(int argc,char **argv){

	int ret; 
    const char *cfname;
    ch_getopt_t *opt;
    ch_pool_t *mp;

    if(argc<2){
        usage();
        udp_exit(-1);
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
        udp_exit(-1);
    }

    /*process commandline opts*/
    ret = ch_getopt_init(&opt,mp,argc,argv);
    
    if(ret!=0){
        usage();
        udp_exit(1);
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
                        udp_exit(0);
                        break;
                     default:
                        usage();
                        udp_exit(1);
                        break;
                }
                break;
            case CH_BADCH:
            case CH_BADARG:
                usage();
                udp_exit(1);

            default:
                break;
        }
    } while (ret != CH_EOF);

    /*create context*/
    udp_work = ch_udp_work_create(mp,cfname);

    if(udp_work == NULL){

        fprintf(stderr,"create context failed from config file%s\n",cfname);
        udp_exit(1);
    }



	ch_signal(SIGINT, handle_signals);
	ch_signal(SIGTERM, handle_signals);

    return ch_udp_work_start(udp_work);
}
