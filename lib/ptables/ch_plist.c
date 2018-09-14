/*
 *
 *      Filename: ch_plist.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-13 11:21:10
 * Last Modified: 2018-09-14 17:37:36
 */

#include "ch_plist.h"
#include "ch_log.h"

ch_plist_t * ch_plist_create(ch_pool_t *mp,int pool_type,
 size_t entry_size,size_t priv_data_size,size_t entry_num_limits,
 uint64_t entry_timeout,
 void *priv_data,
 int (*entry_equal)(ch_plist_entry_t *entry,void *key,void *priv_data),
 void (*entry_clean)(ch_plist_entry_t *entry,void *priv_data),
 void (*entry_timeout_cb)(ch_plist_entry_t *entry,uint64_t tv,void *priv_data)){


	ch_plist_t *plist = (ch_plist_t*)ch_palloc(mp,sizeof(*plist));

	plist->ep = ch_entry_pool_create(mp,pool_type,entry_size+priv_data_size,
		entry_num_limits,0);

	if(plist->ep == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Create entry pool for plist!");
		return NULL;
	}

	INIT_LIST_HEAD(&plist->entries);
	plist->last_entry = NULL;
	plist->entry_size = entry_size;
	plist->priv_data_size = priv_data_size;
	plist->priv_data = priv_data;
	plist->entry_num = 0;
	plist->entry_num_limits = entry_num_limits;
	plist->entry_timeout = entry_timeout;

	plist->last_clean_time = ch_get_current_timems()/1000;
	plist->entry_equal = entry_equal;
	plist->entry_clean = entry_clean;
	plist->entry_timeout_cb = entry_timeout_cb;

	return plist;
}

#define  ENTRY_LAST_TIME_UPDATE(entry) do { \
    entry->last_time = (uint64_t)(ch_get_current_timems()/1000); \
}while(0)

static inline uint64_t _entry_is_timeout(ch_plist_t *plist,ch_plist_entry_t *plist_entry,uint64_t cur_time){

	uint64_t tv = cur_time-plist_entry->last_time;

    if(tv>plist->entry_timeout)
        return tv;

    return 0;
}

static inline void _entry_free(ch_plist_t *plist,ch_plist_entry_t *plist_entry,uint64_t is_timeout){

    list_del(&plist_entry->node);
    
    if(plist_entry == plist->last_entry)
        plist->last_entry = NULL;

	if(is_timeout){
		if(plist->entry_timeout_cb){
		
			plist->entry_timeout_cb(plist_entry,is_timeout,plist->priv_data);
		}
	}
	if(plist->entry_clean)
		plist->entry_clean(plist_entry,plist->priv_data);

	ch_entry_pool_free(plist->ep,plist_entry);

}

static inline void _entries_timeout_free(ch_plist_t *plist){

	uint64_t tv;
    uint64_t cur_time = (uint64_t)(ch_get_current_timems()/1000);
    
	ch_plist_entry_t *entry,*tmp_entry;
	
	list_for_each_entry_safe(entry,tmp_entry,&plist->entries,node){
		tv = _entry_is_timeout(plist,entry,cur_time);
		if(tv){
			_entry_free(plist,entry,tv);
		}
	}

}



ch_plist_entry_t * ch_plist_entry_create(ch_plist_t *plist){

    ch_plist_entry_t *plist_entry = NULL;

	plist_entry = ch_entry_pool_alloc(plist->ep);

    if(plist_entry == NULL){
    
        ch_log(CH_LOG_WARN,
			"Cannot alloc new plist entry ,try to free some timeout entries and try again --------------");

        _entries_timeout_free(plist);
	
		plist_entry = ch_entry_pool_alloc(plist->ep);

        if(plist_entry == NULL){
       
            ch_log(CH_LOG_ERR,"The number of entries allocated has been overflow,no entries timeout are freed!");
    
            return NULL;
        }
    }

	ENTRY_LAST_TIME_UPDATE(plist_entry);
	list_add(&plist_entry->node,&plist->entries);
	plist->entry_num += 1;
	plist->last_entry = plist_entry;

    return plist_entry;

}

void  ch_plist_entry_free(ch_plist_t *plist,ch_plist_entry_t *entry){

	_entry_free(plist,entry,0);
}

#define ENTRY_IS_EQUAL(plist,entry,key) plist->entry_equal(entry,key,plist->priv_data) 

ch_plist_entry_t * ch_plist_entry_find(ch_plist_t *plist,void *key){

	ch_plist_entry_t *plist_entry = NULL,*entry;

	if(plist->last_entry&&ENTRY_IS_EQUAL(plist,plist->last_entry,key))
		plist_entry = plist->last_entry;
	else{
	
		list_for_each_entry(entry,&plist->entries,node){

			if(ENTRY_IS_EQUAL(plist,entry,key))
			{
				plist_entry = entry;
				break;
			}
		}
	}

	if(plist_entry){
	
		ENTRY_LAST_TIME_UPDATE(plist_entry);
		list_del(&plist_entry->node);
		list_add(&plist_entry->node,&plist->entries);
		plist->last_entry = plist_entry;
	}

	return plist_entry;
}

ch_plist_entry_t * ch_plist_entry_find_create(ch_plist_t *plist,void *key){

	ch_plist_entry_t *plist_entry = ch_plist_entry_find(plist,key);
	if(plist_entry == NULL){
	
		plist_entry = ch_plist_entry_create(plist);
	}

	return plist_entry;
}

size_t ch_plist_entries_timeout_free(ch_plist_t *plist,
	int (*need_free)(ch_plist_entry_t *entry,void *priv_data)){

	size_t c = 0;
    uint64_t cur_time = ch_get_current_timems()/1000; 
	uint64_t tv;

	ch_plist_entry_t *entry,*tmp_entry;

	if((cur_time-plist->last_clean_time)<plist->entry_timeout)
		return 0;

	plist->last_clean_time = cur_time;
	
	list_for_each_entry_safe(entry,tmp_entry,&plist->entries,node){

		tv = _entry_is_timeout(plist,entry,cur_time);

		if(tv||(need_free&&need_free(entry,plist->priv_data))){
			c+=1;
			_entry_free(plist,entry,tv);
		}
	}


	return c;
}

void ch_plist_dump(ch_plist_t *plist,FILE *fp){

	fprintf(fp,"Dump Plist Informations:\n");
	fprintf(fp,"plist.entry.num:%lu\n",plist->entry_num);

	ch_entry_pool_dump(plist->ep,fp);

}
