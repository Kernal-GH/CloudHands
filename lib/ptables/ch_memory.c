/*
 *
 *      Filename: ch_memory.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-27 10:12:11
 * Last Modified: 2018-06-21 13:27:21
 */

#include <rte_malloc.h>
#include "ch_memory.h"
#include "ch_log.h"

ch_memory_t* ch_memory_create(ch_pool_t *mp,size_t max_cache_size,uint64_t timeout){


	ch_memory_t *mm;
	ch_memory_cache_t *mc = NULL;

	mm = (ch_memory_t*)ch_palloc(mp,sizeof(*mm));

	mm->use_size = 0;
	mm->use_sys_size = 0;
	mm->use_dpdk_size = 0;

	mm->cache_hits_size = 0;
	mm->mp = mp;

	if(max_cache_size>0){
	
	
		mc = ch_memory_cache_create(mm,max_cache_size,timeout);
		if(mc == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot create memory cache for memory!");
			return NULL;
		}
	}

	mm->mcache = mc;

	return mm;
}

void* do_memory_alloc(ch_memory_t *mm,size_t size,int is_zero){


	uint16_t type;
	void *addr;
	ch_memory_item_t *item;

	size_t rsize = ch_align64pow2(size+sizeof(ch_memory_item_t));

	/*Try to alloc form cache*/
	if(mm->mcache){
	
		item = ch_memory_cache_find(mm->mcache,rsize);
		if(item){
		
			item->size = size;
			item->rsize = rsize;
			item->from_cache = 1;
			mm->cache_hits_size += rsize;

			return (void*)(item+1);
		}
	}

	/*Try to alloc from dpdk*/
	type = FROM_DPDK;
	addr = rte_malloc(NULL,rsize,0);
	if(addr == NULL){
	
		addr = malloc(rsize);
		type = FROM_SYS;
	}

	if(addr == NULL)
		return NULL;

	if(is_zero)
		memset(addr,0,rsize);

	item = (ch_memory_item_t*)addr;
	item->m_protect = M_PROTECT_V;
	item->type = type;
	item->size = size;
	item->rsize = rsize;
	item->from_cache = 0;

	mm->use_size+=rsize; 
	
	if(type == FROM_SYS){
	
		mm->use_sys_size += rsize;
	}else{
	
		mm->use_dpdk_size += rsize;
	}

	return (void*)(item+1);

}

void* ch_memory_alloc(ch_memory_t *mm,size_t size){

	return do_memory_alloc(mm,size,0);
}

void* ch_memory_calloc(ch_memory_t *mm,size_t size){

	return do_memory_alloc(mm,size,1);
}

#define CH_MEMORY_ITEM_CHECK(item) do { \
	if((item)->m_protect!=M_PROTECT_V)  \
	{\
		ch_log(CH_LOG_ERR,"memory item is bad:%d,type:%d,from_cache:%d,size:%d,rsize:%d",\
			item->m_protect,item->type,item->from_cache,item->size,item->rsize);\
		abort();						\
	}\
}while(0)


void  ch_memory_free(ch_memory_t *mm,void *addr){


	ch_memory_item_t *item = (ch_memory_item_t*)(addr-sizeof(*item));
	size_t rsize = item->rsize;
	CH_MEMORY_ITEM_CHECK(item);

	if(mm->mcache){
	
		if(ch_memory_cache_update(mm->mcache,item)==0){
		
			return;
		}
	}

	switch(item->type){
	
	case FROM_SYS:
		mm->use_sys_size-=rsize;
		free((void*)item);
		break;

	case FROM_DPDK:
		mm->use_dpdk_size -=rsize;
		rte_free((void*)item);
		break;

	default:
		break;
	}

	mm->use_size -= rsize;
}

void ch_memory_item_free(ch_memory_t *mm,ch_memory_item_t *item){

	size_t rsize = item->size;
	CH_MEMORY_ITEM_CHECK(item);

	switch(item->type){
	
	case FROM_SYS:
		mm->use_sys_size-=rsize;
		free((void*)item);
		break;

	case FROM_DPDK:
		mm->use_dpdk_size -=rsize;
		rte_free((void*)item);
		break;
	
	default:
		ch_log(CH_LOG_ERR,"Invalid memory item!");
		break;
	}

	mm->use_size -= rsize;

}

void ch_memory_dump(ch_memory_t *mm,FILE *fp){


	fprintf(fp,"Dump memory info------------------------------------\n");
	fprintf(fp,"Using Total memory size:%lu\n",(unsigned long)mm->use_size);
	fprintf(fp,"Using System Total memory size:%lu\n",(unsigned long)mm->use_sys_size);
	fprintf(fp,"Using DPDK Total memory size:%lu\n",(unsigned long)mm->use_dpdk_size);
	fprintf(fp,"Cache hits Total memory size:%lu\n",(unsigned long)mm->cache_hits_size);
	
	if(mm->mcache){
	
		ch_memory_cache_dump(mm->mcache,fp);
	}
}
