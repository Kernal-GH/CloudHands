/*
 *
 *      Filename: ch_sa_data_store_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-06 13:44:48
 * Last Modified: 2018-06-06 13:44:48
 */

#ifndef CH_SA_DATA_STORE_POOL_H
#define CH_SA_DATA_STORE_POOL_H

typedef struct ch_sa_data_store_pool_t ch_sa_data_store_pool_t;
typedef struct ch_sa_data_store_t ch_sa_data_store_t;

#include "ch_mpool.h"
#include "ch_list.h"

struct ch_sa_data_store_pool_t {

	struct list_head dstore_free_list;

	ch_pool_t *mp;

	uint32_t free_n;
	uint32_t using_n;

	uint32_t allac_dstore_n;

	uint32_t dsize;
	uint32_t dstore_limits;
};

struct ch_sa_data_store_t {

	struct list_head node;
	
	ch_sa_data_store_pool_t *dstore_pool;

	void *start;
	void *pos;
	void *end;
};

#define MAX_DSIZE 512
#define PRE_ALLOC_N 1000

extern ch_sa_data_store_pool_t * ch_sa_data_store_pool_create(uint32_t dsize,uint32_t dstore_limits);

#define ch_sa_data_store_get(dsp,dst) do {															\
	dst = NULL;																						\
	if(list_empty(&((dsp)->dstore_free_list))&&((dsp)->allac_dstore_n<(dsp)->dstore_limits)){		\
		dst = (ch_sa_data_store_t*)ch_palloc((dsp)->mp,sizeof(ch_sa_data_store_t)+(dsp)->dsize+32);	\
		(dst)->dstore_pool = dsp;																	\
		(dst)->start = (void*)((dst)+1);															\
		(dst)->pos = (dst)->start;																	\
		(dst)->end = (dst)->start+(dsp)->dsize;														\
		(dsp)->using_n =  (dsp)->using_n+1;													        \
		(dsp)->allac_dstore_n+=1;																	\
	}else{																							\
		dst = list_first_entry(&((dsp)->dstore_free_list),ch_sa_data_store_t,node);					\
		list_del(&((dst)->node));																	\
		(dsp)->free_n = (dsp)->free_n-1;															\
		(dsp)->using_n =  (dsp)->using_n+1;															\
	}																								\
}while(0)

#define ch_sa_data_store_free(dst) do {							\
	ch_sa_data_store_pool_t *dsp = (dst)->dstore_pool;			\
	(dst)->pos = (dst)->start;									\
	list_add_tail(&((dst)->node),&((dsp)->dstore_free_list));		\
	dsp->free_n += 1;											\
}while(0)

#define ch_sa_data_store_full(dst) ((dst)->pos>=(dst)->end)

#define ch_sa_data_store_write(dst,data,dsize) do {   \
	size_t r_dsize = (dst)->end-(dst)->pos;			  \
	if(dsize<r_dsize)								  \
		r_dsize = dsize;							  \
	if(r_dsize){									  \
		memcpy((dst)->pos,data,r_dsize);			  \
		(dst)->pos = (dst)->pos+r_dsize;			  \
	}												  \
}while(0)

#define ch_sa_data_store_size(dst) ((dst)->pos-(dst)->start)

#endif /*CH_SA_DATA_STORE_POOL_H*/
