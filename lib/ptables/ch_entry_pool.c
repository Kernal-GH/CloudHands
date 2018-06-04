/*
 *
 *      Filename: ch_entry_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-28 12:01:39
 * Last Modified: 2017-12-28 18:18:31
 */

#include "ch_entry_pool.h"
#include "ch_log.h"

ch_entry_pool_t * ch_entry_pool_create(ch_pool_t *mp,
	int type,
	size_t entry_size,
	size_t n_entries_limit,
	size_t n_caches_limit){


	ch_entry_pool_t *ep;

	if(entry_size<sizeof(ch_ptable_entry_t))
	{
	
		ch_log(CH_LOG_ERR,"Invalid entry size:%lu",(unsigned long)entry_size);
		return NULL;
	}

	ep = (ch_entry_pool_t*)ch_pcalloc(mp,sizeof(*ep));
	ep->type = type;
	ep->entry_size = entry_size;
	ep->n_entries_limit = n_entries_limit;

	switch(ep->type){
	
	case FROM_OBJ_POOL:
		ep->sys_mp = ch_object_pool_create(mp,n_entries_limit,entry_size,NULL,NULL);
		if(ep->sys_mp == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot Create Object pool for entry pool!");
			return NULL;
		}

		break;

	case FROM_DPDK_POOL:

		ep->dpdk_mp = rte_mempool_create(NULL,
			n_entries_limit,
			entry_size,
			n_caches_limit,0,NULL,NULL,NULL,NULL,0,0);

		if(ep->dpdk_mp == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot Create DPDK Mempool for entry pool!");
			return NULL;
		}

		break;

	default:
		ch_log(CH_LOG_ERR,"Invalid pool type:%d",type);
		return NULL;
	}

	return ep;
}

void ch_entry_pool_destroy(ch_entry_pool_t *ep){

	switch(ep->type){
	
	case FROM_OBJ_POOL:
		ch_object_pool_destroy(ep->sys_mp);
		break;

	case FROM_DPDK_POOL:

		rte_mempool_free(ep->dpdk_mp);

		break;

	default:
		break;
	}

}

ch_ptable_entry_t * ch_entry_pool_alloc(ch_entry_pool_t *ep){

	void *entry = NULL;

	switch(ep->type){
	
	case FROM_OBJ_POOL:

		entry = ch_object_pool_get(ep->sys_mp);

		break;

	case FROM_DPDK_POOL:
		
		if(rte_mempool_get(ep->dpdk_mp,&entry))
			return NULL;

		break;

	default:
		return NULL;
	}

	return (ch_ptable_entry_t*)entry;
}


void ch_entry_pool_free(ch_entry_pool_t *ep,ch_ptable_entry_t *pentry) {

	switch(ep->type){
	
	case FROM_OBJ_POOL:
		ch_object_pool_put(ep->sys_mp,(void*)pentry);

		break;

	case FROM_DPDK_POOL:

		rte_mempool_put(ep->dpdk_mp,(void*)pentry);

		break;

	default:
		break;
	}

}

void ch_entry_pool_dump(ch_entry_pool_t *ep,FILE *fp){


	switch(ep->type){
	
	case FROM_OBJ_POOL:
		ch_object_pool_dump(ep->sys_mp,fp);
		break;

	case FROM_DPDK_POOL:
		rte_mempool_dump(fp,ep->dpdk_mp);
		break;

	default:
		break;
	}
}

