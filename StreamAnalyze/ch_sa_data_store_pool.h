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
#include "ch_log.h"

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

	int ds_protect;

	struct list_head node;
	
	ch_sa_data_store_pool_t *dstore_pool;

	void *start;
	void *pos;
	void *end;
};
#define DS_PROTECT_V 1234

#define MAX_DSIZE 512
#define PRE_ALLOC_N 1000

#define CH_SA_DATA_STORE_CHECK(dst) do {\
	if((dst)->ds_protect!=DS_PROTECT_V){\
		ch_log(CH_LOG_ERR,"SA Data Store is bad,ds_protect:%d",(dst)->ds_protect); \
		abort();\
	}\
}while(0)

extern ch_sa_data_store_pool_t * ch_sa_data_store_pool_create(uint32_t dsize,uint32_t dstore_limits);

static inline ch_sa_data_store_t *ch_sa_data_store_get(ch_sa_data_store_pool_t *dsp)  {											
	
	ch_sa_data_store_t *dst = NULL;	
	struct list_head *free_list = &dsp->dstore_free_list;

	if(dsp->allac_dstore_n>dsp->dstore_limits&&list_empty_careful(free_list)){
	
		ch_log(CH_LOG_ERR,"Cannot get data store,too more memory has been allocated!");
		return NULL;
	}

	if(list_empty_careful(free_list)){		
		dst = (ch_sa_data_store_t*)ch_pcalloc(dsp->mp,sizeof(*dst)+dsp->dsize+32);
		if(dst) {
			dst->ds_protect = DS_PROTECT_V;
			dst->dstore_pool = dsp;																	
			dst->start = (void*)(dst+1);															
			dst->pos = dst->start;																	
			dst->end = dst->start+dsp->dsize;														
			dsp->using_n+=1;													        
			dsp->allac_dstore_n+=1;
		}																	
	}else{																							
		
		dst = list_first_entry(free_list,ch_sa_data_store_t,node);					
		CH_SA_DATA_STORE_CHECK(dst);
		dst->pos = dst->start;
		list_del_init(&dst->node);
		dsp->free_n-=1;															
		dsp->using_n+=1;															
	}

	return dst;
}

static inline void ch_sa_data_store_free(ch_sa_data_store_t *dst) {

	CH_SA_DATA_STORE_CHECK(dst);

	ch_sa_data_store_pool_t *dsp = dst->dstore_pool;			
	dst->pos = dst->start;									
	list_add_tail(&dst->node,&dsp->dstore_free_list);		
	dsp->free_n += 1;
	dsp->using_n -=1;

}


static inline void ch_sa_data_store_write(ch_sa_data_store_t *dst,void *data,size_t dsize) {  

	CH_SA_DATA_STORE_CHECK(dst);
	size_t r_dsize = dst->end-dst->pos;			  

	if(dsize<r_dsize)					
		r_dsize = dsize;

	if(r_dsize){									  
		memcpy(dst->pos,data,r_dsize);			  
		dst->pos = dst->pos+r_dsize;			  
	}												  
}

#define ch_sa_data_store_full(dst) ((dst)->pos>=(dst)->end)
#define ch_sa_data_store_size(dst) ((dst)->pos-(dst)->start)

#endif /*CH_SA_DATA_STORE_POOL_H*/
