/*
 *
 *      Filename: StatDump.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-07 10:32:27
 * Last Modified: 2018-09-27 17:29:37
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ch_stat_pool.h"

static void print_usage(void) {

	const char* usage = "Usage:StatDump <mmapFileName> <cmdName> <cmdArgs>\n"
				   "cmdName and cmdArgs as :\n"
				   "d <tv>\n"
				   "c <stat time range up> <stat time interval>\n";


	printf("%s",usage);

}


static uint64_t to_long(const char *p){

	char *endptr; 
	return (uint64_t)strtoul(p,&endptr,10);
}

int main(int argc,char ** argv){

	ch_stat_pool_t *st_pool;
	const char *mmap_fname;
	const char *cmd;

	if(argc <4){

		print_usage();

		return -1;
	}

	mmap_fname = argv[1];
	cmd = argv[2];

	ch_pool_t *mp = ch_pool_create(4096);

	if(*cmd == 'd'){
		
		st_pool = ch_stat_pool_create(mp,mmap_fname,0,0);
		if(st_pool == NULL){
		
			printf("Cannot open stat pool to dump!\n");
			ch_pool_destroy(mp);
			return -1;
		}

		uint64_t tv =  to_long(argv[3]);
		
		while(1){
		
			ch_stat_pool_dump(st_pool,stdout);

			sleep(tv);
		}

	}else if(*cmd == 'c'){

		if(argc<5){
		
			print_usage();
			ch_pool_destroy(mp);
			return -1;
		}

		uint64_t stat_timeup = to_long(argv[3]);
		uint64_t stat_tv = to_long(argv[4]);

		st_pool = ch_stat_pool_create(mp,mmap_fname,stat_timeup,stat_tv);
		if(st_pool == NULL){
		
			printf("Cannot create stat pool!\n");
			ch_pool_destroy(mp);
			return -1;
		}

	}

	ch_pool_destroy(mp);

	return 0;
}

