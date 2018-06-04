/*
 *
 *      Filename: ch_black_white_list.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-17 19:19:25
 * Last Modified: 2017-11-17 20:31:09
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ch_log.h"
#include "ch_black_white_list.h"

static void _entry_create(ch_black_white_list_t *bwl,char *line){

	char *endptr;
	char *ip_start;
	char *ip_end;
	char *port_start;
	char *port_end;

	uint32_t ip_start_int;
	uint32_t ip_end_int;
	uint16_t port_start_int;
	uint16_t port_end_int;


	ch_bw_entry_t *entry;

	ip_start = strtok(line,",");
	ip_end = strtok(NULL,",");
	port_start = strtok(NULL,",");
	port_end = strtok(NULL,",");

	ip_start_int = ip_start?(uint32_t)strtoul(ip_start,&endptr,10):0;
	ip_end_int = ip_end?(uint32_t)strtoul(ip_end,&endptr,10):0;
	port_start_int = port_start?(uint16_t)strtoul(port_start,&endptr,10):0;
	port_end_int = port_end?(uint16_t)strtoul(port_end,&endptr,10):0;

	if(ip_start_int<=ip_end_int&&port_start_int<=port_end_int){
	
		entry = (ch_bw_entry_t*)apr_pcalloc(bwl->mp,sizeof(*entry));
		entry->ip_start = ip_start_int;
		entry->ip_end = ip_end_int;
		entry->port_start = port_start_int;
		entry->port_end = port_end_int;

		list_add_tail(&entry->node,&bwl->entries);

	}
}

ch_black_white_list_t* ch_black_white_list_load(apr_pool_t *mp,const char *fname){

	char line[1024];
	size_t len;

	ch_black_white_list_t *bwl = (ch_black_white_list_t*)apr_pcalloc(mp,sizeof(*bwl));

	bwl->mp = mp;

	CH_INIT_LIST_HEAD(&bwl->entries);

	FILE *fp = fopen(fname,"r");
	if(fp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot open file:%s",fname);
		return NULL;
	}

	while(fgets(line,1023,fp)){
	
		len = strlen(line);

		if(len<=5||*line=='#')
			continue;

		line[len-1]=0;

		_entry_create(bwl,line);
	}

	return bwl;
}

#define MATCH_RANGE(start,end,v) (((end)==0)||(((v)>=(start))&&((v)<=(end))))

int ch_black_white_list_match(ch_black_white_list_t *bwl,uint32_t ip,uint16_t port){


	ch_bw_entry_t *entry;

	if(list_empty(&bwl->entries))
		return 1;

	list_for_each_entry(entry,&bwl->entries,node){
	
		if(MATCH_RANGE(entry->ip_start,entry->ip_end,ip)&&MATCH_RANGE(entry->port_start,entry->port_end,port))
			return 1;
	}

	return 0;
}
