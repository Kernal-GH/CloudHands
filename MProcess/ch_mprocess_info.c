/*
 *
 *      Filename: ch_mprocess_info.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-11-10 14:04:44
 * Last Modified: 2017-11-10 17:20:06
 */

#include <rte_memzone.h>
#include <rte_lcore.h>
#include "ch_log.h"
#include "ch_mprocess_info.h"

ch_mprocess_info_t * ch_mprocess_info_create(uint64_t size){

	void *base_addr;
	const struct rte_memzone* mz;

	ch_mprocess_info_t *mp_info = NULL;
	uint64_t rsize = size<MPROCESS_MEMSIZE_DEFAULT?MPROCESS_MEMSIZE_DEFAULT:size;

	mz = rte_memzone_reserve(MPROCESS_NAME, rsize,rte_socket_id(), 0);
	if (mz == NULL){
		ch_log(CH_LOG_ERR, "Cannot reserve memory zone for mprocess  information");
	
		return NULL;
	}
	
	base_addr = mz->addr;

	memset(base_addr, 0, rsize);

	mp_info = (ch_mprocess_info_t*)base_addr;
	mp_info->base_addr = base_addr;
	mp_info->size = rsize;
	mp_info->qpool_number = 0;
	mp_info->alloc_offset = sizeof(ch_mprocess_info_t);
	mp_info->qpool_head = 0;

	return mp_info;
}

ch_mprocess_info_t * ch_mprocess_info_get(void){

	const struct rte_memzone *mz;
	ch_mprocess_info_t *mp_info;

	mz = rte_memzone_lookup(MPROCESS_NAME);
	if(mz == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot find mprocess info ,please create it first!");
		return NULL;
	}

	mp_info = (ch_mprocess_info_t*)mz->addr;

	return mp_info;
}

ch_mprocess_queue_pool_info_t* ch_mprocess_queue_pool_info_lookup(ch_mprocess_info_t *mp_info,const char * name){

	ch_mprocess_queue_pool_info_t *qpool_info = NULL;

	list_for_mprocess_queue_pool_info(mp_info,qpool_info){

		if(strcmp(name,qpool_info->name)== 0)
			return qpool_info;
	}

	return NULL;
}

ch_mprocess_queue_pool_info_t* ch_mprocess_queue_pool_info_create(ch_mprocess_info_t *mp_info,const char *name,
	const char *filter_name,const char *hash_name){

	ch_mprocess_queue_pool_info_t *qpool_info = NULL;
	void *addr;

	if(check_name(name)||check_name(filter_name)||check_name(hash_name))
	{
	
		ch_log(CH_LOG_ERR,"String max size:%d",MAX_NAME_SIZE);
		return NULL;
	}
	if(ch_mprocess_queue_pool_info_lookup(mp_info,name)){
	
		ch_log(CH_LOG_ERR,"MProcess name:%s,has existed!",name);
		return NULL;
	}

	addr = ch_mprocess_calloc(mp_info,sizeof(*qpool_info));
	if(addr == NULL){
	
		ch_log(CH_LOG_ERR,"No memory space for create mprocess queue pool!");
		return NULL;
	}
	qpool_info = (ch_mprocess_queue_pool_info_t*)addr;

	strcpy(qpool_info->name,name);
	strcpy(qpool_info->filter_name,filter_name);
	strcpy(qpool_info->hash_name,hash_name);

	qpool_info->queue_number = 0;
	qpool_info->queue_head = 0;
	qpool_info->stats.packets = 0;
	qpool_info->stats.drop_packets = 0;

	/*add to mprocess list*/
	qpool_info->next = mp_info->qpool_head;
	mp_info->qpool_head = ADDR_TO_OFFSET(mp_info,addr);
	mp_info->qpool_number+=1;

	return qpool_info;
}

ch_mprocess_queue_info_t* ch_mprocess_queue_info_lookup(ch_mprocess_info_t *mp_info,ch_mprocess_queue_pool_info_t *qpool_info,const char * name){

	ch_mprocess_queue_info_t *qinfo = NULL;
	list_for_mprocess_queue_info(mp_info,qpool_info,qinfo){
	
		if(strcmp(qinfo->name,name) == 0)
			return qinfo;
	}

	return NULL;
}

ch_mprocess_queue_info_t* ch_mprocess_queue_info_create(ch_mprocess_info_t *mp_info,ch_mprocess_queue_pool_info_t *qpool_info,const char *name,uint32_t queue_size){

	ch_mprocess_queue_info_t *qinfo = NULL;
	void *addr = NULL;

	if(check_name(name))
	{
	
		ch_log(CH_LOG_ERR,"String max size:%d",MAX_NAME_SIZE);
		return NULL;
	}

	if(ch_mprocess_queue_info_lookup(mp_info,qpool_info,name)){
	
		ch_log(CH_LOG_ERR,"MProcess info name:%s,has existed!",name);
		return NULL;
	}

	addr = ch_mprocess_calloc(mp_info,sizeof(*qinfo));
	if(addr == NULL){
	
		ch_log(CH_LOG_ERR,"No memory space for create mprocess queue!");
		return NULL;
	}
	
	qinfo = (ch_mprocess_queue_info_t*)addr;
	strcpy(qinfo->name,name);
	qinfo->queue_size = queue_size;
	qinfo->next = 0;
	qinfo->stats.packets = 0;
	qinfo->stats.drop_packets = 0;

	/*add to mprocess queue pool*/
	qinfo->next = qpool_info->queue_head;
	qpool_info->queue_head = ADDR_TO_OFFSET(mp_info,addr);
	qpool_info->queue_number+=1;

	return qinfo;
}

void ch_mprocess_info_dump(ch_mprocess_info_t *mp_info,FILE *fp){

	ch_mprocess_queue_info_t *qinfo;
	ch_mprocess_queue_pool_info_t *qpool_info;

	if(mp_info == NULL)
		mp_info = ch_mprocess_info_get();

	if(mp_info == NULL)
		return;

	fprintf(fp,"MProcess information dump starting ..........................\n");
	fprintf(fp,"name:%s\n",MPROCESS_NAME);
	fprintf(fp,"memorySize:%lu\n",(unsigned long)mp_info->size);
	fprintf(fp,"queuePoolNumber:%lu\n",(unsigned long)mp_info->qpool_number);
	fprintf(fp,"MProcess Queue Pool information dump starting -------------------------\n");

	list_for_mprocess_queue_pool_info(mp_info,qpool_info){

		fprintf(fp,"++++Name:%s\n",qpool_info->name);
		fprintf(fp,"++++filterName:%s\n",qpool_info->filter_name);
		fprintf(fp,"++++hashName:%s\n",qpool_info->hash_name);
		fprintf(fp,"++++queueNumber:%lu\n",(unsigned long)qpool_info->queue_number);
		fprintf(fp,"++++packets:%lu\n",(unsigned long)qpool_info->stats.packets);
		fprintf(fp,"++++dropPackets:%lu\n",(unsigned long)qpool_info->stats.drop_packets);

		list_for_mprocess_queue_info(mp_info,qpool_info,qinfo){
		
		
			fprintf(fp,"++++++++Name:%s\n",qinfo->name);
			fprintf(fp,"++++++++queueSize:%lu\n",(unsigned long)qinfo->queue_size);
			fprintf(fp,"++++++++packets:%lu\n",(unsigned long)qinfo->stats.packets);
			fprintf(fp,"++++++++dropPackets:%lu\n",(unsigned long)qinfo->stats.drop_packets);
		}
	}

}
