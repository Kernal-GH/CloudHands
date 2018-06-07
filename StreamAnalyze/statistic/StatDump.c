/*
 *
 *      Filename: StatDump.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-07 10:32:27
 * Last Modified: 2018-06-07 14:44:29
 */


#include <stdio.h>
#include <stdlib.h>
#include "ch_stat_pool.h"
#include "ch_log.h"

int main(int argc,char ** argv){

	ch_stat_pool_t *st_pool;
	const char *mmap_fname;
	int tv;

	if(argc <3){
	
		printf("usage:<prg> <mmap_fname> <tv>\n");
		return -1;
	}

	mmap_fname = argv[1];

	tv = atoi(argv[2]);

	ch_pool_t *mp = ch_pool_create(4096);


	ch_log_init(mp,"/tmp/StatDump.log",CH_LOG_DEBUG);

	
	st_pool = ch_stat_pool_create(mp,mmap_fname,0,0);

	while(1){
	
		ch_stat_pool_dump(st_pool,stdout);

		sleep(tv);
	}

	return 0;
}

