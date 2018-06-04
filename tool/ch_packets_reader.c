/*
 *
 *      Filename: ch_packets_reader.c
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-06-01 11:23:41
 * Last Modified: 2017-06-01 12:13:22
 */


#include <stdio.h>
#include <stdlib.h>
#include <apr_pools.h>
#include <unistd.h>
#include <apr_signal.h>

#include "ch_log.h"
#include "ch_mmap_packets_format.h"

static int is_exit = 0;

/**
 * Handle signals.
 */
static void handle_signals(int signum)
{
    switch (signum) {
        case SIGINT:
            printf("Caught SIGINT, shutting down.\n");
            exit(0);
        case SIGTERM:
            printf("Caught SIGTERM, shutting down.\n");
            exit(0);
        default:
            break;
    }

	is_exit = 1;

    printf("Caught unexpected signal %d\n", signum);
    exit(1);
}

static void read_packets_interface(ch_mmap_packets_format_t *pfmt){

	ch_mmap_packets_iterator_t *iter = NULL;
	ch_mmap_packets_entry_t *pe = NULL; 

	while(!is_exit) {
		
		iter = ch_mmap_packets_format_iterator_prefare(pfmt);
		
		if (iter == NULL) {
			ch_log(CH_LOG_INFO,"%s: iter is null, sleep %d seconds.", __func__, 1);
			
			usleep(1000000);

			continue;
		}

		while((pe = iter->next(iter))){

			ch_mmap_packets_entry_dump(pe,stdout);
		}

        ch_mmap_packets_format_iterator_commit(pfmt, iter);
	}

	/* exit */
	is_exit = 0;

	if (pfmt && iter)
		ch_mmap_packets_format_iterator_commit(pfmt, iter);

	return 0;

}

int main(int argc,char ** argv){

	const char *fname;
	apr_pool_t *mp;
	int rc;
	ch_mmap_packets_format_t *pfmt;

	if(argc<2){
	
		fprintf(stderr,"Usage: appName <packets interface file name>!\n");
		return -1;
	}

	fname = argv[1];

    /*init apr app runtine*/
    apr_app_initialize(&argc,&argv,NULL);

    /*create global apr memory pool*/
    apr_pool_create(&mp,NULL);

    if(mp == NULL){

        fprintf(stderr,"create global memory pool failed!\n");
        return -1;
    }

    /*init errno*/
    rc = ch_strerror_init();
    if(rc!=0){
        fprintf(stderr,"init errno failed!\n");
        return -1;
    }
    
    /*init log*/
    ch_log_init(mp,"/tmp/preader.log",CH_LOG_DEBUG);

	apr_signal(SIGINT, handle_signals);
	apr_signal(SIGTERM, handle_signals);

	pfmt = ch_mmap_packets_format_create(mp,fname,0,0,0);
	if(pfmt == NULL){

		fprintf(stderr,"Create packets mmap file format instance failed!\n");
		return -1;
	}

	read_packets_interface(pfmt);

	return 0;
}

