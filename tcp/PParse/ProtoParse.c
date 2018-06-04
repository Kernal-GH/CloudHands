/*
 *
 *      Filename: ProtoParse.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 15:48:50
 * Last Modified: 2018-05-17 18:59:15
 */

#include "ch_getopt.h"
#include "ch_log.h"
#include "ch_mpool.h"
#include "ch_signal.h"
#include "ch_pp_work.h"

#define CMDLINE_OPTS "h"

static ch_pp_work_t *pwork;

/**
 * Usage text.
 */
static void usage(void) {
    fprintf(stderr, "  Usage: PPMain [options] <path to the config file> <shm file name>\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  Options:\n");
    fprintf(stderr, "    -h        This help\n\n");
}


static void pp_exit(int rc){


	ch_pp_work_stop(pwork);

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
            pp_exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            pp_exit(0);
        default:
            break;
    }


    printf("Caught unexpected signal %d\n", signum);
    pp_exit(1);
}

int main(int argc,char **argv){

	int ret;
    const char *cfname;
	const char *shm_fname;

    ch_getopt_t *opt;
    ch_pool_t *mp;

    if(argc<3){
        usage();
		return -1;
    }
    
    cfname = argv[1];
	shm_fname = argv[2];

    /*create global apr memory pool*/
    mp = ch_pool_create(4096);

    if(mp == NULL){

        fprintf(stderr,"create global memory pool failed!\n");
        return -1;
    }

    /*process commandline opts*/
    ret = ch_getopt_init(&opt,mp,argc,argv);
    
    if(ret!=0){
        usage();
        return -1;
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
                        pp_exit(0);
                        break;
                     default:
                        usage();
                        pp_exit(1);
                        break;
                }
                break;
            case CH_BADCH:
            case CH_BADARG:
                usage();
                pp_exit(1);

            default:
                break;
        }
    } while (ret != CH_EOF);

    /*create work*/
    pwork = ch_pp_work_create(mp,cfname,shm_fname);

    if(pwork == NULL){

        fprintf(stderr,"create proto work failed from config file%s\n",cfname);
        pp_exit(1);
    }


	ch_signal(SIGINT, handle_signals);
	ch_signal(SIGTERM, handle_signals);

    return ch_pp_work_start(pwork);

}
