/*
 *
 *      Filename: SMonCmd.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-11 17:11:31
 * Last Modified: 2018-07-12 10:41:53
 */


#include <stdio.h>
#include <stdlib.h>
#include "ch_session_monitor.h"

static void print_usage(void){

	const char* usage = "Usage:SMonCmd <mmapFileName> <mmsize> [cmdName] [cmdArgs]\n"
				   "cmdName and cmdArgs as :\n"
				   "D\n"
				   "a 0 <ip> <tv>\n"
				   "a 1 <port> <tv>\n"
				   "a 2 <ip> <port> <tv>\n"
				   "a 3 <sip> <sport> <dip> <dport> <tv>\n"
				   "f <sip> <sport> <dip> <dport>\n"
				   "f <id>\n"
				   "d <id>\n"
				   "s <id>\n"
				   "r <id> \n";

	printf("%s",usage);

}

static uint64_t to_long(const char *p){

	char *endptr; 
	return (uint64_t)strtoul(p,&endptr,10);
}

static int _run_find(ch_session_monitor_t *monitor,int argc,char **argv){

	uint32_t src_ip,dst_ip;
	uint16_t src_port,dst_port;

	uint64_t id;

	ch_session_monitor_item_t *item = NULL;

	if(argc == 1){

		id = to_long(argv[0]);
		item = ch_session_monitor_item_findById(monitor,id);
		if(item == NULL){
		
			printf("Cannot find monitor item by id:%lu\n",(unsigned long)id);
		}else{
			printf("Find a monitor item by id:%lu\n",(unsigned long )id);
		
			ch_session_monitor_item_dump(item,stdout);
		}

		return 0;
	}

	if(argc == 4){
	
		src_ip = (uint32_t)to_long(argv[0]);
		src_port = (uint16_t)to_long(argv[1]);
		dst_ip = (uint32_t)to_long(argv[2]);
		dst_port = (uint16_t)to_long(argv[3]);

		item = ch_session_monitor_item_find(monitor,
			src_ip,dst_ip,src_port,dst_port);

		if(item == NULL){
		
			printf("Cannot find monitor item by session:(%lu,%lu,%lu,%lu)\n",(unsigned long)src_ip,
				(unsigned long)src_port,
				(unsigned long)dst_ip,
				(unsigned long)dst_port);

		}else{
			
			printf("Find a monitor item by session:(%lu,%lu,%lu,%lu)\n",(unsigned long)src_ip,
				(unsigned long)src_port,
				(unsigned long)dst_ip,
				(unsigned long)dst_port);
		
			ch_session_monitor_item_dump(item,stdout);
		}
		
		return 0;
	}

	printf("Invalid monitor find args!\n");
	print_usage();

	return -1;
}

static int _run_add(ch_session_monitor_t *monitor,int argc,char **argv){

	int type;
	uint64_t tv;

	if(argc<3){
	
		printf("Invalid add item args!\n");
		print_usage();
		return -1;
	}

	type = atoi(argv[0]);

	if(type<=-1||type>3){
	
		printf("Invalid add item type:%d\n",type);
		print_usage();
		return -1;
	}

	int rc = -1;

	switch(type){
	
	case 0:
		if(argc!=3){
		
			printf("Invalid args add item by ip!\n");
			print_usage();
			rc = -1;
		}else{
			uint32_t ip = (uint32_t)to_long(argv[1]);
			tv = to_long(argv[2]);

			rc = (int)ch_session_monitor_item_add_ip(monitor,ip,tv);

		}

		break;

	case 1:
		if(argc!=3){
		
			printf("Invalid args add item by port!\n");
			print_usage();
			rc = -1;
		}else{
			uint16_t port = (uint16_t)to_long(argv[1]);
			tv = to_long(argv[2]);

			rc = (int)ch_session_monitor_item_add_port(monitor,port,tv);
		}

		break;

	case 2:
		if(argc!=4){
		
			printf("Invalid args add item by ip and port!\n");
			print_usage();
			rc = -1;
		}else{
            uint32_t ip = (uint32_t)to_long(argv[1]);
			uint16_t port = (uint16_t)to_long(argv[2]);
			tv = to_long(argv[3]);
			rc = (int)ch_session_monitor_item_add_ip_port(monitor,ip,port,tv);
		}
		break;

	case 3:
		if(argc!=6){
		
			printf("Invalid args add item by session!\n");
			print_usage();
			rc = -1;
		}else{
            uint32_t sip = (uint32_t)to_long(argv[1]);
			uint16_t sport = (uint16_t)to_long(argv[2]);
            uint32_t dip = (uint32_t)to_long(argv[3]);
			uint16_t dport = (uint16_t)to_long(argv[4]);

			tv = to_long(argv[5]);

			rc = (int)ch_session_monitor_item_add_session(monitor,sip,dip,sport,dport,tv);
		}
		break;

	default:
		break;
	}

    if(rc<12345){
    
        printf("add a item into monitor failed!\n");

    }else{
    
        printf("add a item into monitor ok,id:%d\n",rc);
    }
    return rc;
}

static int _parse_rum_cmd(ch_session_monitor_t *monitor,int argc,char **argv){

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
		ch_session_monitor_dump(monitor,stdout);
		rc = 0;
		break;

	case 'a':
        printf("Start to add:\n");
        rc = _run_add(monitor,argc,argv+p);
		break;

	case 'f':
		printf("Start to find:\n");
		rc = _run_find(monitor,argc,argv+p);
		break;

	case 'd':
		if(argc!=1){
		
			printf("Invalid delete cmd args!\n");
			print_usage();
			rc = -1;
		}else{
        
			id = to_long(argv[1]);
        
			printf("Start to delete:%lu\n",(unsigned long)id);
		
		
			ch_session_monitor_item_del(monitor,id);
		
			rc = id;
		}

		break;

	case 's':
		
		if(argc!=1){
		
			printf("Invalid stop cmd args!\n");
			print_usage();
			rc = -1;
		}else{
			id = to_long(argv[1]);
			printf("Start to stop:%lu\n",(unsigned long)id);
	
			ch_session_monitor_item_stop(monitor,id);
			rc = id;
		}

		break;

	case 'r':
		
		if(argc!=1){
		
			printf("Invalid restart cmd args!\n");
			print_usage();
			rc = -1;
		}else{
			id = to_long(argv[1]);
			printf("Start to restart:%lu\n",(unsigned long)id);
			ch_session_monitor_item_restart(monitor,id);
			rc = id;
		}

		break;

	default:
		printf("Unknown cmd:%s\n",cmd);
		print_usage();
		rc = -1;
	}

    return rc;
}

int main(int argc,char ** argv){

	ch_session_monitor_t tmp,*monitor = &tmp;

	const char *mmap_fname;
	size_t msize;
	

	if(argc <3){
	
		print_usage();
		return -1;
	}

	mmap_fname = argv[1];
	msize = (size_t)to_long(argv[2]);

	if(ch_session_monitor_load(monitor,mmap_fname,msize)){
	

		printf("Cannot load monitor,mmapFile:%s,mmsize:%lu\n",mmap_fname,(unsigned long)msize);
		return -1;
	}

	argc -= 3;
	argv += 3;

	if(argc){
	
		return _parse_rum_cmd(monitor,argc,argv);
	}

    return 0;
}

