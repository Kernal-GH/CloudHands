/*
 *
 *      Filename: ch_session_monitor.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-11 11:08:14
 * Last Modified: 2018-07-19 19:26:53
 */

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "ch_session_monitor.h"
#include "ch_util.h"
#include "ch_net_util.h"

static int _mmap_file_open(const char *fname,uint64_t fsize,int existed) {
   
	int fd;

   if(existed){
	   
	   return open(fname,O_RDWR,0644);
   }

   fd = open(fname,O_RDWR|O_CREAT,0644);
   if(fd <0){
      return fd;
   }
   /*alloc space file*/
   if(fallocate(fd,0,0,fsize)){
      return -1;
   }

   /*ok*/
   return fd;
}

static uint64_t _get_fsize(const char *mmap_fname){

	 struct stat statbuf;  
	 stat(mmap_fname,&statbuf);  
	 
	 return statbuf.st_size;  
}


static void * _mmap_file_attach(int fd,uint64_t r_msize){

	return  mmap(NULL,r_msize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

}

#define ITEM_INIT(item) do {                 \
	(item)->check_start = CHECK_VALUE;		 \
	(item)->id = 0;							 \
	(item)->src_ip = 0;						 \
	(item)->dst_ip = 0;						 \
	(item)->src_port = 0;					 \
	(item)->dst_port = 0;					 \
	(item)->monitor_time_tv = 0;			 \
	(item)->monitor_start_time = 0;			 \
	(item)->monitor_last_time = 0;			 \
	(item)->monitor_state = MON_STATE_INIT;  \
}while(0)

int ch_session_monitor_load(ch_session_monitor_t *monitor,const char *mmap_fname,size_t msize){

	int existed = 0;
	uint64_t r_msize;
	uint64_t pg_size = sysconf(_SC_PAGE_SIZE); 
	uint64_t fsize = 0;
	void *addr_start;
	void *addr_end;
	ch_session_monitor_item_t *cur_item;
	ch_session_monitor_hdr_t *hdr;

	/*align to pg_size*/
	r_msize =(((msize)+(pg_size-1))&~(pg_size-1));

	if(mmap_fname == NULL){
	
		fprintf(stdout,"Please specify the mmap file path!\n");
		return -1;
	}
	
	existed = (access(mmap_fname,F_OK) == 0);

	if(!existed&&msize==0){
	
		fprintf(stdout,"Please specify the mmap size,mmapFile:%s\n",mmap_fname);
		return -1;
	}
	if(existed){
	
		fsize = _get_fsize(mmap_fname);
		if(fsize == 0){
		
			fprintf(stdout,"mmap filesize is zero!\n");
			return -1;
		}
		r_msize = fsize;
	}


	monitor->fd = _mmap_file_open(mmap_fname,r_msize,existed);
	if(monitor->fd <0){
	
		fprintf(stdout,"Cannot open file:%s ",mmap_fname);
		return -1;
	}

	addr_start = _mmap_file_attach(monitor->fd,r_msize);
	if(addr_start == NULL){

		fprintf(stdout,"cannot load mmapFile into memory:%s\n",mmap_fname);
		close(monitor->fd);
		return -1;
	}

	addr_end = addr_start+r_msize;
	monitor->monitor_hdr = (ch_session_monitor_hdr_t*)addr_start;
	monitor->monitor_items = (ch_session_monitor_item_t*)(monitor->monitor_hdr+1);


	if(existed){
		MON_VALUE_CHECK(monitor->monitor_hdr->check_start);
		MON_VALUE_CHECK(monitor->monitor_items->check_start);
	}else{
		
		hdr = monitor->monitor_hdr;
		cur_item =  monitor->monitor_items;

		hdr->check_start = CHECK_VALUE;
		hdr->item_number = 0;
		hdr->next_id = ID_INIT_VALUE;

		while(((void*)(cur_item+1))<addr_end){
		

			hdr->item_number += 1;
			ITEM_INIT(cur_item);
			cur_item += 1;
		}

	}

	/*ok*/
	return 0;
}

void ch_session_monitor_exit(ch_session_monitor_t *monitor){

	close(monitor->fd);

}

static const char * _state_string_get(int state){

	switch(state){
	
	case MON_STATE_INIT:
		return "init";

	case MON_STATE_START:
		return "start";

	case MON_STATE_LIVING:
		return "living";

	case MON_STATE_STOP:
		return "stop";

	default:
		return "unknown";
	}

}

static const char * _type_string_get(int type){

	switch(type){
	
	case MON_ITEM_TYPE_PORT:
		return "moni.port";

	case MON_ITEM_TYPE_IP:
		return "moni.ip";

	case MON_ITEM_TYPE_IP_PORT:
		return "moni.ip.port";

	case MON_ITEM_TYPE_SESSION:
		return "moni.session";

	default:
		return "unknown";
	}

}

void ch_session_monitor_item_dump(ch_session_monitor_item_t *cur_item,FILE *fp){

	
	char buf[64] = {0};
	fprintf(fp,"item.id:%lu\n",(unsigned long)cur_item->id);
	fprintf(fp,"item.srcIP:%s\n",ch_ip_to_str(buf,63,cur_item->src_ip));
	fprintf(fp,"item.dstIP:%s\n",ch_ip_to_str(buf,63,cur_item->dst_ip));

	fprintf(fp,"item.srcPort:%lu\n",(unsigned long)cur_item->src_port);
	fprintf(fp,"item.dstPort:%lu\n",(unsigned long)cur_item->dst_port);
	fprintf(fp,"item.monitor_time_tv:%lu\n",(unsigned long)cur_item->monitor_time_tv);
	fprintf(fp,"item.monitor_start_time:%lu\n",(unsigned long)cur_item->monitor_start_time);
	fprintf(fp,"item.monitor_last_time:%lu\n",(unsigned long)cur_item->monitor_last_time);
	fprintf(fp,"item.monitor_state:%s\n",_state_string_get(cur_item->monitor_state));
	fprintf(fp,"item.monitor_type:%s\n",_type_string_get(cur_item->monitor_type));

}

void ch_session_monitor_dump(ch_session_monitor_t *monitor,FILE *fp){

	uint32_t i = 0;


	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;
	ch_session_monitor_item_t *cur_item = monitor->monitor_items;

	fprintf(fp,"Dump session monitir informations:\n");
	fprintf(fp,"item.number:%lu\n",(unsigned long)hdr->item_number);
	fprintf(fp,"item.nextID:%lu\n",(unsigned long)hdr->next_id);

	fprintf(fp,"Dump the Monitor items informations:\n");
	while((i++)<hdr->item_number&&cur_item->monitor_state!=MON_STATE_INIT){
	
		ch_session_monitor_item_dump(cur_item,fp);
		cur_item += 1;
	}


}

static inline int is_match(ch_session_monitor_item_t *item,
	uint32_t src_ip,uint16_t src_port,uint32_t dst_ip,uint16_t dst_port){

	switch(item->monitor_type){

	case MON_ITEM_TYPE_PORT:
		return item->dst_port == src_port || item->dst_port == dst_port;

	case MON_ITEM_TYPE_IP:
		return item->dst_ip == src_ip || item->dst_ip == dst_ip;

	case MON_ITEM_TYPE_IP_PORT:
		return (item->dst_ip == src_ip&&item->dst_port == src_port)||(item->dst_ip == dst_ip&&item->dst_port == dst_port);

	case MON_ITEM_TYPE_SESSION:
		return (item->src_ip == src_ip&&item->src_port == src_port&&item->dst_ip == dst_ip&&item->dst_port == dst_port)||\
			   (item->src_ip == dst_ip&&item->src_port == dst_port&&item->dst_ip == src_ip&&item->dst_port == src_port);

	default:
		return 0;
	}


}

ch_session_monitor_item_t * ch_session_monitor_item_find(ch_session_monitor_t *monitor,
	uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port){


	uint32_t i = 0;
	ch_session_monitor_item_t *item = NULL,*cur_item = monitor->monitor_items;
	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;

	while((i++)<hdr->item_number&&cur_item->monitor_state!=MON_STATE_INIT){

		if(cur_item->monitor_state == MON_STATE_STOP){
		
			cur_item += 1;

			continue;
		
		}

		if(!is_match(cur_item,src_ip,src_port,dst_ip,dst_port)){
	
			cur_item += 1;
			continue;
		}

		if(item == NULL){
		
			item = cur_item;
		}else{
			if(item->monitor_type<cur_item->monitor_type){
			
				item = cur_item;
			}
		}
		cur_item += 1;
	}

	return item;
}

ch_session_monitor_item_t * ch_session_monitor_item_get(ch_session_monitor_t *monitor,int index){

	ch_session_monitor_item_t *item,*cur_item = monitor->monitor_items;
	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;

	if(index>=(int)hdr->item_number)
		return NULL;

	item = cur_item+index;
	if(item->monitor_state == MON_STATE_INIT)
		return NULL;

	return item;
}

int ch_session_monitor_item_count(ch_session_monitor_t *monitor){

	ch_session_monitor_item_t *item,*cur_item = monitor->monitor_items;
	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;

	uint32_t i = 0;
	
	int c = 0;

	while((i++)<hdr->item_number&&cur_item->monitor_state!=MON_STATE_INIT){

		c++;
		cur_item++;
	}

	return c;
}

ch_session_monitor_item_t * ch_session_monitor_item_find_ignore_state(ch_session_monitor_t *monitor,
	uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port){


	uint32_t i = 0;
	ch_session_monitor_item_t *item = NULL,*cur_item = monitor->monitor_items;
	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;

	while((i++)<hdr->item_number&&cur_item->monitor_state!=MON_STATE_INIT){

		if(!is_match(cur_item,src_ip,src_port,dst_ip,dst_port)){
	
			cur_item += 1;
			continue;
		}

		if(item == NULL){
		
			item = cur_item;
		}else{
			if(item->monitor_type<cur_item->monitor_type){
			
				item = cur_item;
			}
		}
		cur_item += 1;
	}

	return item;
}

static ch_session_monitor_item_t * _alloc_item(ch_session_monitor_t *monitor){

	uint32_t i = 0;

	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;
	ch_session_monitor_item_t *cur_item = monitor->monitor_items;

	while((i++)<hdr->item_number){

		if(cur_item->monitor_state == MON_STATE_INIT){
		
			return cur_item;
		}
		cur_item += 1;
	}

	return NULL;
}

static uint64_t _time_get(void)
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

uint64_t _item_add(ch_session_monitor_t *monitor,int type,
	uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port,
	uint64_t monitor_time_tv){

	ch_session_monitor_item_t *item;
	
	item = ch_session_monitor_item_find(monitor,src_ip,dst_ip,src_port,dst_port);

	if(item!=NULL&&item->monitor_type == type)
	{
		fprintf(stdout,"has been added!\n");
		return item->id;
	}

	item = _alloc_item(monitor);
	if(item == NULL){
	
		fprintf(stdout,"no free memory space to add item!\n");
		return 1;
	}

	item->check_start = CHECK_VALUE;
	item->id = ch_session_monitor_alloc_id(monitor);
	item->src_ip = src_ip;
	item->dst_ip = dst_ip;
	item->src_port = src_port;
	item->dst_port = dst_port;
	item->monitor_time_tv = monitor_time_tv;
	item->monitor_start_time = _time_get()/1000;
	item->monitor_last_time = item->monitor_start_time;
	item->monitor_type = type;
	item->monitor_state = MON_STATE_START;

	return item->id;

}

uint64_t ch_session_monitor_item_add_ip(ch_session_monitor_t *monitor,uint32_t ip,uint64_t monitor_time_tv){


	if(ip == 0){
	
		fprintf(stdout,"Invalid ip:0\n");
		return 0;
	}

	return _item_add(monitor,MON_ITEM_TYPE_IP,0,ip,0,0,monitor_time_tv);

}

uint64_t ch_session_monitor_item_add_port(ch_session_monitor_t *monitor,uint16_t port,uint64_t monitor_time_tv){


	if(port == 0){
	
		fprintf(stdout,"Invalid port:0\n");
		return 0;
	}

	return _item_add(monitor,MON_ITEM_TYPE_PORT,0,0,0,port,monitor_time_tv);

}

uint64_t ch_session_monitor_item_add_ip_port(ch_session_monitor_t *monitor,uint32_t ip,uint16_t port,uint64_t monitor_time_tv){

	if(ip == 0 || port == 0)
	{
		fprintf(stdout,"Invalid args:(IP:%lu,Port:%lu)\n",(unsigned long)ip,(unsigned long)port);
		return 0;
	}

	return _item_add(monitor,MON_ITEM_TYPE_IP_PORT,0,ip,0,port,monitor_time_tv);

}

uint64_t ch_session_monitor_item_add_session(ch_session_monitor_t *monitor,
	uint32_t src_ip,uint32_t dst_ip,uint16_t src_port,uint16_t dst_port,
	uint64_t monitor_time_tv){


	if(src_ip == 0 || src_port == 0 || dst_ip == 0 || dst_port == 0)
	{
		fprintf(stdout,"Invalid args:(src_ip:%lu,src_port:%lu,dst_ip:%lu,dst_port:%lu)\n",
			(unsigned long)src_ip,(unsigned long)src_port,
			(unsigned long)dst_ip,(unsigned long)dst_port);
		return 0;
	}

	return _item_add(monitor,MON_ITEM_TYPE_SESSION,src_ip,dst_ip,src_port,dst_port,monitor_time_tv);

}

ch_session_monitor_item_t * ch_session_monitor_item_findById(ch_session_monitor_t *monitor,uint64_t id){

	uint32_t i = 0;
	ch_session_monitor_item_t *cur_item = monitor->monitor_items;
	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;

	while((i++)<hdr->item_number&&cur_item->monitor_state!=MON_STATE_INIT){
	
		if(cur_item->id == id)
			return cur_item;

		cur_item += 1;
	}

	return NULL;
}

#define ITEM_COPY(d_item,s_item) do {							\
	(d_item)->check_start = (s_item)->check_start;				\
	(d_item)->id = (s_item)->id;								\
	(d_item)->src_ip = (s_item)->src_ip;						\
	(d_item)->dst_ip = (s_item)->dst_ip;						\
	(d_item)->src_port = (s_item)->src_port;					\
	(d_item)->dst_port = (s_item)->dst_port;					\
	(d_item)->monitor_time_tv = (s_item)->monitor_time_tv;		\
	(d_item)->monitor_start_time = (s_item)->monitor_start_time;\
	(d_item)->monitor_last_time = (s_item)->monitor_last_time;	\
	(d_item)->monitor_type = (s_item)->monitor_type;			\
	(d_item)->monitor_state = (s_item)->monitor_state;			\
}while(0)

static void elements_move(ch_session_monitor_t *monitor,ch_session_monitor_item_t *start_item){


	uint32_t start_index = ch_session_monitor_item_index(monitor,start_item);
	uint32_t i =  start_index+1;

	ch_session_monitor_item_t *cur_item = start_item+1;

	ch_session_monitor_hdr_t *hdr = monitor->monitor_hdr;


	while((i++)<hdr->item_number&&cur_item->monitor_state!=MON_STATE_INIT){

		ITEM_COPY(cur_item-1,cur_item);
		cur_item++;
	}
	
	ITEM_INIT(cur_item-1);

}

void ch_session_monitor_item_del(ch_session_monitor_t *monitor,uint64_t id){

	ch_session_monitor_item_t *item = ch_session_monitor_item_findById(monitor,id);

	if(item){

		elements_move(monitor,item);

	}
}

void ch_session_monitor_item_stop(ch_session_monitor_t *monitor,uint64_t id){

	ch_session_monitor_item_t *item = ch_session_monitor_item_findById(monitor,id);

	if(item){
	
		item->monitor_state = MON_STATE_STOP;
	}

}

void ch_session_monitor_item_restart(ch_session_monitor_t *monitor,uint64_t id){

	ch_session_monitor_item_t *item = ch_session_monitor_item_findById(monitor,id);

	if(item){
	
		item->monitor_state = MON_STATE_START;
	}

}






