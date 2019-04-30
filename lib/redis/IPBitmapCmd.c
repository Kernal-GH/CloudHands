/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/29/2019 02:58:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ch_redis_ipbitmap.h"

static void print_usage(void) {

	const char* usage = "Usage:IPTbBitmapCmd <host> <port> <passwd> <iptab_name> <cmd> [cmdArgs]\n"
				   "cmd and cmdArgs as :\n"
				   "s <ip>\n"
				   "u <ip>\n"
				   "g <ip>\n"
				   "r \n"
				   "d\n";

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

static void _run_set_cmd(ch_redis_t *redis,const char *iptab_name,int argc,char **argv){

    int rc;

    if(argc<1){
        printf("please specify ip!\n");
        print_usage();
        exit(-1);
    }
    uint32_t ip = _ip_str_to_int(argv[0]);

    rc = ch_redis_ipbitmap_set(redis,iptab_name,ip);
    if(rc==0){

        printf("add ip:%s to iptable:%s ok!\n",argv[0],iptab_name);
    }else{

        printf("add ip:%s to iptable:%s failed!\n",argv[0],iptab_name);
    }
}

static void _run_unset_cmd(ch_redis_t *redis,const char *iptab_name,int argc,char **argv){

    int rc;

    if(argc<1){
        printf("please specify ip!\n");
        print_usage();
        exit(-1);
    }
    uint32_t ip = _ip_str_to_int(argv[0]);

    rc = ch_redis_ipbitmap_unset(redis,iptab_name,ip);
    if(rc==0){

        printf("remove ip:%s from iptable:%s ok!\n",argv[0],iptab_name);
    }else{

        printf("remove ip:%s from iptable:%s failed!\n",argv[0],iptab_name);
    }

}

static void _run_get_cmd(ch_redis_t *redis,const char *iptab_name,int argc,char **argv){

    int rc;

    if(argc<1){
        printf("please specify ip!\n");
        print_usage();
        exit(-1);
    }
    uint32_t ip = _ip_str_to_int(argv[0]);

    rc = ch_redis_ipbitmap_is_in(redis,iptab_name,ip);
    if(rc==1){

        printf("ip:%s is in iptable:%s!\n",argv[0],iptab_name);
    }else{

        printf("ip:%s is not in iptable:%s\n",argv[0],iptab_name);
    }
}

static void _run_reset_cmd(ch_redis_t *redis,const char *iptab_name){

    int rc;

    rc = ch_redis_ipbitmap_reset(redis,iptab_name);
    if(rc==0){

        printf("reset iptable:%s ok!\n",iptab_name);
    }else{

        printf("reset iptable:%s failed!\n",iptab_name);
    }

}


static void _run_del_cmd(ch_redis_t *redis,const char *iptab_name){

    ch_redis_ipbitmap_del(redis,iptab_name);
    printf("del iptable:%s is ok!\n",iptab_name);

}

static void  _parse_run_cmd(ch_redis_t *redis,const char *iptab_name,int argc,char **argv){

    const char *cmd;
	int p = 0;

	cmd = argv[p];
	p++;
	argc--;

	switch(*cmd){

	case 's':
		printf("Start to add cmd:\n");
		_run_set_cmd(redis,iptab_name,argc,argv+p);
        break;

	case 'u':
        printf("Start to remove cmd:\n");
        _run_unset_cmd(redis,iptab_name,argc,argv+p);
		break;

	case 'g':

        printf("Start to get cmd:\n");
        _run_get_cmd(redis,iptab_name,argc,argv+p);
		break;
	
	case 'r':
        printf("Start to reset cmd:\n");
        _run_reset_cmd(redis,iptab_name);
		break;

	case 'd':
        printf("Start to del cmd:\n");
        _run_del_cmd(redis,iptab_name);

		break;

	default:
		printf("Unknown cmd:%s\n",cmd);
		print_usage();
	}

}

int main(int argc,char **argv){

    ch_redis_t redis,*redis_ptr = &redis;

    if(argc<6){

        print_usage();
        exit(-1);
    }


    int rc = ch_redis_init(redis_ptr,argv[1],atoi(argv[2]),argv[3]);
    if(rc){

        printf("Cannot connect to redis:%s:%s\n",argv[1],argv[2]);
        return -1;
    }


    const char *iptab_name = argv[4];

	argc -= 5;
	argv += 5;

    _parse_run_cmd(redis_ptr,iptab_name,argc,argv);

    ch_redis_free(redis_ptr);

    return 0;
}
