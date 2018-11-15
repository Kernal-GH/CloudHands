/*
 *
 *      Filename: WBListCmd.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-05 15:41:37
 * Last Modified: 2018-09-06 19:04:13
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ch_wb_list_str.h"

static void print_usage(void) {

	const char* usage = "Usage:WBListStrCmd <mmapFileName> <mmsize> <max String Len> [cmdName] [cmdArgs]\n"
				   "cmdName and cmdArgs as :\n"
				   "D\n"
				   "a <str>\n"
				   "d <id>\n"
				   "m <str> \n"
				   "e <value>";

	printf("%s",usage);

}

static uint64_t to_long(const char *p){

	char *endptr; 
	return (uint64_t)strtoul(p,&endptr,10);
}

static int _run_add(ch_wb_list_t *wb_list,int argc,char **argv){

	uint64_t id;

	if(argc<1){
	
		printf("Invalid add entry args!\n");
		print_usage();
		return -1;
	}

	id = ch_wb_list_add(wb_list,(void*)argv[0]);

	if(id==0){
		printf("add failed!\n");
	}else{
	
		printf("add ok,id:%lu\n",(unsigned long)id);
	}

    return id;
}

static int _run_match(ch_wb_list_t *wb_list,int argc,char **argv){

	if(argc<1){
	
		printf("Invalid match cmd args!\n");
		print_usage();
		return -1;
	}


	int rc = ch_wb_list_is_match(wb_list,(void*)argv[0]);

	if(rc==0){
		printf("unmatch!\n");
	}else{
	
		printf("match!\n");
	}

    return rc;
}

static int _parse_rum_cmd(ch_wb_list_t *wb_list,int argc,char **argv){

	uint64_t id;
    const char *cmd;
	int p = 0;
	int rc = 0;

	cmd = argv[p];
	p++;
	argc--;

	switch(*cmd){

	case 'D':
		printf("Start to Dump:\n");
		ch_wb_list_dump(wb_list,stdout);
		rc = 0;
		break;

	case 'a':
        printf("Start to add:\n");
        rc = _run_add(wb_list,argc,argv+p);
		break;

	case 'd':
		if(argc!=1){
		
			printf("Invalid delete cmd args!\n");
			print_usage();
			rc = -1;
		}else{
        
			id = to_long(argv[1]);
        
			printf("Start to delete:%lu\n",(unsigned long)id);
			ch_wb_list_del(wb_list,id);

			rc = id;
		}

		break;
	
	case 'e':
		if(argc!=1){
		
			printf("Invalid enable/disable wblist cmd args!\n");
			print_usage();
			rc = -1;
		}else{
        
			int vv = (int)to_long(argv[1]);
        
			printf("Start to enable/disable wblist:%d\n",vv);

			ch_wb_list_is_on_set(wb_list,vv);

			rc = 0;
		}

		break;

	case 'm':
        printf("Start to match:\n");
        rc = _run_match(wb_list,argc,argv+p);
		break;

	default:
		printf("Unknown cmd:%s\n",cmd);
		print_usage();
		rc = -1;
	}

    return rc;
}

int main(int argc,char **argv){

	ch_wb_list_t wb_list;

	const char *mmap_fname;
	size_t msize;
	size_t max_str_len;

	if(argc<4)
	{
		print_usage();
		return -1;
	}

	mmap_fname = argv[1];
	msize = (size_t)to_long(argv[2]);
	max_str_len = (size_t)to_long(argv[3]);

	if(ch_wb_list_str_init(&wb_list,mmap_fname,msize,max_str_len))
	{
		printf("Cannot init wb list str,mmapFile:%s,mmsize:%lu\n",mmap_fname,(unsigned long)msize);
		return -1;
	}

	argc -= 4;
	argv += 4;

	if(argc){
	
		return _parse_rum_cmd(&wb_list,argc,argv);
	}

    return 0;

}


