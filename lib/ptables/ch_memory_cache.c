/*
 *
 *      Filename: ch_memory_cache.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-03 11:39:25
 * Last Modified: 2018-06-21 16:23:17
 */

#include "ch_memory_cache.h"
#include "ch_util.h"

static inline size_t  _get_index(size_t size){

	size_t i = 0;
	while(i<=MAX_CACHE_POW&&(size>(size_t)(1<<i))) i++;

	return i;
}


ch_memory_cache_t* ch_memory_cache_create(ch_memory_t *mm,size_t max_cache_size,uint64_t timeout){


	ch_memory_cache_t *mc;
	int i;

	ch_memory_cache_header_t *hc;

	mc = (ch_memory_cache_t*)ch_palloc(mm->mp,sizeof(*mc));

	mc->max_cache_size = max_cache_size<MAX_CACHE_SIZE_D?MAX_CACHE_SIZE_D:max_cache_size;

	mc->cur_cache_size = 0;

	mc->timeout = timeout;

	mc->mm = mm;

	for(i = 0;i<MAX_CACHE_INDEX;i++){
	
		hc = &mc->cache_headers[i];
		hc->n_hits = 0;
		hc->n_nohits = 0;
		hc->n_caches = 0;
		hc->last_find_time = (uint64_t)(ch_get_current_timems()/1000); 
		
		INIT_LIST_HEAD(&hc->items);

	}

	return mc;
}

void ch_memory_cache_reset(ch_memory_cache_t *mc){

	ch_memory_cache_header_t *ch;
	ch_memory_item_t *mitem,*tmp;

	mc->cur_cache_size = 0;
	int i;

	for(i = 0;i<MAX_CACHE_INDEX;i++){
	
		ch = &mc->cache_headers[i];
		ch->n_hits = 0;
		ch->n_nohits = 0;
		ch->n_caches = 0;
		ch->last_find_time = (uint64_t)(ch_get_current_timems()/1000); 


		list_for_each_entry_safe(mitem,tmp,&ch->items,node){

			list_del(&mitem->node);
			ch_memory_item_free(mc->mm,mitem);

		}

		INIT_LIST_HEAD(&ch->items);

	}
}

ch_memory_item_t * ch_memory_cache_find(ch_memory_cache_t *mc,size_t size){


	ch_memory_cache_header_t *ch;
	ch_memory_item_t *mitem;


	size_t i = _get_index(size);
	if(i>=MAX_CACHE_INDEX){
	
		return NULL;
	}

	ch = &mc->cache_headers[i];
	ch->last_find_time = (uint64_t)(ch_get_current_timems()/1000); 
	if(list_empty(&ch->items)){		
		
		ch->n_nohits += 1;
		return NULL;
	}

	mitem = list_first_entry(&ch->items,ch_memory_item_t,node);
	list_del(&mitem->node);

	ch->n_hits += 1;
	ch->n_caches -= 1;

	return mitem;
}

static inline int _should_update(ch_memory_cache_t *mc,ch_memory_cache_header_t *ch){

	uint64_t cur_time = (uint64_t)(ch_get_current_timems()/1000);

	if(ch->n_caches == 0)
		return 1;

	if(cur_time-ch->last_find_time>mc->timeout)
		return 0;


	return ch->n_nohits>ch->n_caches;
}

int ch_memory_cache_update(ch_memory_cache_t *mc,ch_memory_item_t *mitem){


	ch_memory_cache_header_t *ch;
	size_t i;

	if(mc->cur_cache_size+mitem->rsize>mc->max_cache_size)
		ch_memory_cache_reset(mc);

	i = _get_index(mitem->rsize);
	if(i>=MAX_CACHE_INDEX){

		return -1;
	}

	ch = &mc->cache_headers[i];  
	
	if(!_should_update(mc,ch))
	{
	
		return -1;
	}

	ch->n_caches += 1;

	list_add(&mitem->node,&ch->items);

	if(!mitem->from_cache){
	

		mc->cur_cache_size += mitem->rsize;
	}

	/*ok*/
	return 0;
}

static inline void _dump_cache_header(ch_memory_cache_header_t *ch,FILE *fp,int i){

	uint64_t cur_time = (uint64_t)(ch_get_current_timems()/1000);

	fprintf(fp,"**********Dump Cache Header:%d\n",i);
	fprintf(fp,"{hits:%lu,nohits:%lu,caches:%lu,totalSize:%lu,before:%d seconds been fould!}\n",
		(unsigned long)ch->n_hits,
		(unsigned long)ch->n_nohits,
		(unsigned long)ch->n_caches,
		(unsigned long)(ch->n_caches*(1<<i)),
		(int)(cur_time-ch->last_find_time));
}

void ch_memory_cache_dump(ch_memory_cache_t *mc,FILE *fp){


	fprintf(fp,"Dump Memory Cache!\n");
	fprintf(fp,"max cache size:%lu\n",(unsigned long)(mc->max_cache_size));
	fprintf(fp,"current cache size:%lu\n",(unsigned long)(mc->cur_cache_size));
	fprintf(fp,"timeout:%lus\n",(unsigned long)mc->timeout);

	int i;
	ch_memory_cache_header_t *ch;

	for(i = 0;i<MAX_CACHE_INDEX;i++){
	
		ch = &mc->cache_headers[i];

		if(ch->n_caches>0&&(ch->n_hits+ch->n_nohits)>0){
		
			_dump_cache_header(ch,fp,i);
		}
	}
}
