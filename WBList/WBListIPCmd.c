/*
 *
 *      Filename: WBListCmd.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-05 15:41:37
 * Last Modified: 2018-09-06 19:04:15
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ch_wb_list_ip.h"

static void print_usage(void) {

	const char* usage = "Usage:WBListIPCmd <mmapFileName> <mmsize> [cmdName] [cmdArgs]\n"
				   "cmdName and cmdArgs as :\n"
				   "D\n"
				   "a <ip_start> <ip_end> <netmask> <port_start> <port_end>\n"
				   "d <id>\n"
				   "m <ip> <port>\n"
				   "e <value>\n";

	printf("%s",usage);

}

static uint32_t _ip_str_to_int(char *ip){
	
	char*temp = strtok(ip,".");

	uint32_t ip_v=0,part_v = 0;
    int i = 0;
    while(temp)
    {
		part_v = atoi(temp);
		ip_v += part_v<<((8*i));
        temp = strtok(NULL,".");
		i++;
    }

	return ip_v;
}

static uint64_t to_long(const char *p){

	char *endptr; 
	return (uint64_t)strtoul(p,&endptr,10);
}

static int _run_add(ch_wb_list_t *wb_list,int argc,char **argv){

	uint64_t id;

	ch_wb_list_ip_add_value_t t,*av=&t;

	if(argc<5){
	
		printf("Invalid add entry args!\n");
		print_usage();
		return -1;
	}

	av->ip_start = _ip_str_to_int(argv[0]);
	av->ip_end = _ip_str_to_int(argv[1]);
	av->netmask = _ip_str_to_int(argv[2]);
	av->port_start = (uint16_t)to_long(argv[3]);
	av->port_end = (uint16_t)to_long(argv[4]);

	id = ch_wb_list_add(wb_list,(void*)av);
	if(id==0){
		printf("add failed!\n");
	}else{
	
		printf("add ok,id:%lu\n",(unsigned long)id);
	}

    return id;
}

static int _run_match(ch_wb_list_t *wb_list,int argc,char **argv){

	ch_wb_list_ip_match_value_t t,*mv=&t;

	if(argc<2){
	
		printf("Invalid match cmd args!\n");
		print_usage();
		return -1;
	}

	mv->ip = _ip_str_to_int(argv[0]);
	mv->port = (uint16_t)to_long(argv[1]);


	int rc = ch_wb_list_is_match(wb_list,(void*)mv);

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

	if(argc<3)
	{
		print_usage();
		return -1;
	}

	mmap_fname = argv[1];
	msize = (size_t)to_long(argv[2]);

	if(ch_wb_list_ip_init(&wb_list,mmap_fname,msize))
	{
		printf("Cannot init wb list ip,mmapFile:%s,mmsize:%lu\n",mmap_fname,(unsigned long)msize);
		return -1;
	}

	argc -= 3;
	argv += 3;

	if(argc){
	
		return _parse_rum_cmd(&wb_list,argc,argv);
	}

    return 0;

}


