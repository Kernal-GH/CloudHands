/*
 *
 *      Filename: ch_sa_data_store_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-06 15:22:26
 * Last Modified: 2018-06-21 16:39:08
 */

#include "ch_sa_data_store_pool.h"
#include "ch_log.h"

ch_sa_data_store_pool_t * ch_sa_data_store_pool_create(uint32_t dsize,uint32_t dstore_limits) {

	uint32_t r_dsize = dsize<MAX_DSIZE?dsize:MAX_DSIZE;
	int i;
	ch_pool_t *mp;
	ch_sa_data_store_pool_t *dstore_pool;
	ch_sa_data_store_t *dstore;

	mp = ch_pool_create(64*1024);

	if(mp == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot alloc memory for data store pool!");
		return NULL;
	}

	dstore_pool = (ch_sa_data_store_pool_t*)ch_palloc(mp,sizeof(*dstore_pool));

	dstore_pool->mp = mp;
	INIT_LIST_HEAD(&dstore_pool->dstore_free_list);
	dstore_pool->free_n = 0;
	dstore_pool->using_n = 0;
	dstore_pool->allac_dstore_n = 0;
	dstore_pool->dsize = r_dsize;
	dstore_pool->dstore_limits = dstore_limits;

	for(i = 0;i <PRE_ALLOC_N;i++){
	
		dstore = (ch_sa_data_store_t*)ch_palloc(mp,sizeof(ch_sa_data_store_t)+dstore_pool->dsize+32);

		if(dstore == NULL)
			break;
		
		dstore->ds_protect = DS_PROTECT_V;

		dstore->dstore_pool = dstore_pool;																	
		dstore->start = (void*)(dstore+1);															
		dstore->pos = dstore->start;																	
		dstore->end = dstore->start+dstore_pool->dsize;							
		dstore_pool->allac_dstore_n+=1;	
		dstore_pool->free_n += 1;
		list_add_tail(&dstore->node,&dstore_pool->dstore_free_list);
	}

	return dstore_pool;
}
