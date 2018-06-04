/*
 *
 *      Filename: ch_table.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-20 22:32:06
 * Last Modified: 2016-10-21 04:46:52
 */

#include "ch_log.h"
#include "ch_table.h"

static inline int _tbl_size_check(uint32_t tbl_size,uint32_t n_entries_limit){

    if(tbl_size == 0){
    
        ch_log(CH_LOG_ERR," Table size must >0");
        return 1;
    }

    if(!ch_is_power_of_2(tbl_size)){
    
        ch_log(CH_LOG_ERR," Table size must 2 power!");
        return 1;
    }

    if(tbl_size>n_entries_limit){
    
        ch_log(CH_LOG_ERR," Table size too large!");
        return 1;
    }

    return 0;
}

static inline void _table_stats_init(ch_table_stat_t *stat){

    stat->create_num = 0;
    stat->free_num = 0;
    stat->hits_num = 0;
    stat->no_hits_num = 0;
    stat->fail_num = 0;
    stat->timeout_num = 0;
}

static inline struct ch_list_head * entry_header(ch_table_t *tbl,uint32_t idx){

    return tbl->entries+idx;
}

static inline void _asseble_table_entries_init(ch_table_t *tbl,uint32_t tbl_size){

    uint32_t i ;
    struct ch_list_head *h;

    for(i = 0; i<tbl_size; i++){
    
        h = entry_header(tbl,i);
        CH_INIT_LIST_HEAD(h);
    }
}


ch_table_t * ch_table_create(apr_pool_t *mp,
        uint32_t entry_size,uint32_t n_entries_limit,uint32_t tbl_size,
        uint64_t entry_timeout,uint32_t n_caches_limits,
        void *priv_data,
        uint32_t (*entry_hash)(void *key,void *priv_data),
        int (*entry_equal)(ch_table_entry_t *entry,void *key,void *priv_data),
        void (*entry_clean)(ch_table_entry_t *entry,void *priv_data),
		ch_table_entry_t * (*entry_create)(void *key,void *priv_data)){

    uint32_t r_tbl_size,tbl_mask;
    size_t msize = 0;

    ch_table_t *tbl = NULL;

    r_tbl_size = ch_align32pow2(tbl_size);

    /*check failed!*/
    if(_tbl_size_check(r_tbl_size,n_entries_limit))
        return NULL;

    tbl_mask = r_tbl_size -1;
    msize = sizeof(*tbl)+r_tbl_size*sizeof(tbl->entries[0]);

    tbl = (ch_table_t*)apr_palloc(mp,msize);
    if(tbl == NULL){
    
        ch_log(CH_LOG_ERR,"No space used to create assemble table!");
        return NULL;
    }

    tbl->mp = mp;
    tbl->last_entry = NULL;
    tbl->entry_timeout = entry_timeout;
    tbl->entry_size = entry_size;
    tbl->n_entries = 0;
    tbl->n_entries_limit = n_entries_limit;
    tbl->tbl_size = r_tbl_size;
    tbl->tbl_mask = tbl_mask;
    tbl->n_caches = 0;
    tbl->n_caches_limits = n_caches_limits;
    tbl->priv_data = priv_data;
    tbl->entry_hash = entry_hash;
    tbl->entry_equal = entry_equal;
    tbl->entry_clean = entry_clean;
	tbl->entry_create = entry_create;

	tbl->last_clean_time = ch_get_current_timems()/1000;

    _table_stats_init(&tbl->tbl_stats);
    CH_INIT_LIST_HEAD(&tbl->free_entries);
    CH_INIT_LIST_HEAD(&tbl->caches);
    _asseble_table_entries_init(tbl,r_tbl_size);


    return tbl;
}

static inline void lru_node_init(struct ch_list_head *node){

    node->prev = NULL;
    node->next = NULL;
}

static inline int lru_node_empty(struct ch_list_head *node){

    return node->prev == NULL && node->next == NULL;
}

static inline void tbl_entry_free(ch_table_t *tbl,ch_table_entry_t *tbl_entry){

    list_del(&tbl_entry->entry);
    
    if(!lru_node_empty(&tbl_entry->lru)){
        list_del(&tbl_entry->lru);
        tbl->n_caches-=1;
    }

    if(tbl_entry == tbl->last_entry)
        tbl->last_entry = NULL;

	if(tbl->entry_create == NULL)
		list_add(&tbl_entry->entry,&tbl->free_entries);
	else
		tbl->n_entries -=1;
}

static inline void tbl_entry_ok_free(ch_table_t *tbl,ch_table_entry_t *tbl_entry){

    tbl_entry_free(tbl,tbl_entry);
    
	if(tbl->entry_create == NULL)
		tbl->tbl_stats.free_num +=1;
	else{
	
		if(tbl->entry_clean)
			tbl->entry_clean(tbl_entry,tbl->priv_data);
	}

}

static void tbl_entry_timeout_free(ch_table_t *tbl,ch_table_entry_t *tbl_entry){

    tbl_entry_free(tbl,tbl_entry);
    tbl->tbl_stats.timeout_num += 1;

    if(tbl->entry_clean){
    
        tbl->entry_clean(tbl_entry,tbl->priv_data);
    }
}

static inline void _entry_last_time_update(ch_table_entry_t *tbl_entry){

    tbl_entry->last_time = (uint64_t)(ch_get_current_timems()/1000); 
}

static inline int _entry_is_timeout(ch_table_t *tbl,ch_table_entry_t *tbl_entry){

    uint64_t cur_time = (uint64_t)(ch_get_current_timems()/1000);

    if(cur_time-tbl_entry->last_time>tbl->entry_timeout)
        return 1;

    return 0;
}

/*free entries timeout */
size_t ch_table_entries_timeout_free(ch_table_t *tbl,
	int (*need_free)(ch_table_entry_t *entry,void *priv_data)){

	size_t c = 0;

    struct ch_list_head *h;
    uint32_t i;
    uint64_t cur_time = ch_get_current_timems()/1000; 

	ch_table_entry_t *entry,*tmp_entry;

	if((cur_time-tbl->last_clean_time)<tbl->entry_timeout)
		return 0;

	tbl->last_clean_time = cur_time;

	for(i = 0; i<tbl->tbl_size; i++){
    
        h = entry_header(tbl,i);
        
        list_for_each_entry_safe(entry,tmp_entry,h,entry){
       
            if(_entry_is_timeout(tbl,entry)||(need_free&&need_free(entry,tbl->priv_data))){
				c+=1;
                tbl_entry_timeout_free(tbl,entry);
            }
        }
    }


	return c;
}

void _entries_timeout_free(ch_table_t *tbl){

    struct ch_list_head *h;
    uint32_t i;
    ch_table_entry_t *entry,*tmp_entry;

	for(i = 0; i<tbl->tbl_size; i++){
    
        h = entry_header(tbl,i);
        
        list_for_each_entry_safe(entry,tmp_entry,h,entry){
       
            if(_entry_is_timeout(tbl,entry)){
                tbl_entry_timeout_free(tbl,entry);
            }
        }
    }

}

static inline int _is_need_free_timeout_entries(ch_table_t *tbl){

	if(tbl->entry_create)
		return tbl->n_entries_limit<=tbl->n_entries;

    return (tbl->n_entries_limit<=tbl->n_entries)&&(list_empty(&tbl->free_entries));
}

static inline ch_table_entry_t * try_get_entry_from_free_list(ch_table_t *tbl){

    struct ch_list_head *h = &tbl->free_entries;
    ch_table_entry_t *tbl_entry = NULL;
    if(list_empty(h))
        return NULL;

    tbl_entry = list_entry(h->next,ch_table_entry_t,entry);
    list_del(&tbl_entry->entry);

    return tbl_entry;
}

static inline ch_table_entry_t * get_entry_from_mempool(ch_table_t *tbl){

    ch_table_entry_t *tbl_entry = NULL;

    tbl_entry = (ch_table_entry_t*)apr_palloc(tbl->mp,tbl->entry_size);
    if(!tbl_entry){
    
        tbl->tbl_stats.fail_num +=1;
        return NULL;
    }
    tbl->n_entries+=1;
    tbl->tbl_stats.create_num +=1;
    return tbl_entry;
}

static inline struct ch_list_head* _table_header_get(ch_table_t *tbl,void *key){

    uint32_t hash = tbl->entry_hash(key,tbl->priv_data);

    return entry_header(tbl,hash&tbl->tbl_mask);
}

static inline void _table_insert(ch_table_t *tbl,ch_table_entry_t *tbl_entry,
        void *key){

    struct ch_list_head *h = _table_header_get(tbl,key);

    /*insert into header*/
    list_add(&tbl_entry->entry,h);

}

static inline void _table_caches_insert(ch_table_t *tbl,ch_table_entry_t *tbl_entry){

    ch_table_entry_t *tmp_entry;
    if(tbl->n_caches>= tbl->n_caches_limits){
    
        tmp_entry = list_last_entry(&tbl->caches,ch_table_entry_t,lru);
        list_del(&tmp_entry->lru);
        lru_node_init(&tmp_entry->lru);

    }else{
    
        tbl->n_caches+=1;
    }

    list_add(&tbl_entry->lru,&tbl->caches);

}

static void _table_entry_init(ch_table_t *tbl,ch_table_entry_t *tbl_entry,
        void *key){

   
    _entry_last_time_update(tbl_entry);

    tbl->last_entry = tbl_entry;

    _table_insert(tbl,tbl_entry,key);
    _table_caches_insert(tbl,tbl_entry);
}

static inline ch_table_entry_t * _tbl_entry_create(ch_table_t *tbl,void *key){

    ch_table_entry_t *tbl_entry = NULL;

    tbl_entry = tbl->entry_create(key,tbl->priv_data);
    
    if(!tbl_entry){
    
        return NULL;
    }

    tbl->n_entries+=1;
    return tbl_entry;
}

ch_table_entry_t * ch_table_entry_create(ch_table_t *tbl,void *key){

    ch_table_entry_t *tbl_entry = NULL;

    if(_is_need_free_timeout_entries(tbl)){
    
        ch_log(CH_LOG_WARN,
			"The number of entries allocated has been overflow:[%lu,%lu],try to free timeout entries",tbl->n_entries,
                tbl->n_entries_limit);

        _entries_timeout_free(tbl);

        if(_is_need_free_timeout_entries(tbl)){
       
            tbl->tbl_stats.fail_num +=1;
            ch_log(CH_LOG_ERR,"The number of entries allocated has been overflow,no entries timeout are freed!");
    
            return NULL;
        }
    }
  
	if(tbl->entry_create == NULL){
		tbl_entry = try_get_entry_from_free_list(tbl);
		
		if(tbl_entry == NULL){
		
			tbl_entry = get_entry_from_mempool(tbl);
		}
	}else{
	
		tbl_entry = _tbl_entry_create(tbl,key);
	}

    if(tbl_entry){
    
        _table_entry_init(tbl,tbl_entry,key);
    }


    return tbl_entry;
}

void ch_table_entry_free(ch_table_t *tbl,ch_table_entry_t *entry){
 
    tbl_entry_ok_free(tbl,entry);
}

static inline int _table_entry_equal(ch_table_t *tbl,ch_table_entry_t *tbl_entry,
        void *key){

    return tbl->entry_equal(tbl_entry,key,tbl->priv_data);
}

static ch_table_entry_t * _table_entry_find_from_caches(ch_table_t *tbl,void *key){

    ch_table_entry_t *tbl_entry = NULL;
    ch_table_entry_t *tbl_entry_fist = NULL;

    int found = 0;
    struct ch_list_head *h = &tbl->caches;

    list_for_each_entry(tbl_entry,h,lru){
    
        if(_table_entry_equal(tbl,tbl_entry,key))
        {
        
            found = 1;
            break;
        }

    }

    if(found){
    
        tbl->tbl_stats.hits_num +=1;
        tbl_entry_fist = list_first_entry(h,ch_table_entry_t,lru);
        if(tbl_entry_fist!=tbl_entry){
        
            list_del(&tbl_entry->lru);
            list_add(&tbl_entry->lru,h);
        }
        tbl->last_entry = tbl_entry;
    }else{
        tbl_entry = NULL;
    }
    return tbl_entry;
}

static inline ch_table_entry_t * _table_entry_find(ch_table_t *tbl,void *key){

    struct ch_list_head *h = _table_header_get(tbl,key); 
    ch_table_entry_t *tbl_entry = NULL;
    int found = 0;

    list_for_each_entry(tbl_entry,h,entry){
    
        if(_table_entry_equal(tbl,tbl_entry,key)){

            found = 1;
            break;
        }
    }

    if(found){
    
        _table_caches_insert(tbl,tbl_entry);
        tbl->last_entry = tbl_entry;
        tbl->tbl_stats.no_hits_num += 1;
    }else{
    
        tbl_entry = NULL;
    }

    return tbl_entry;
}

ch_table_entry_t * ch_table_entry_find(ch_table_t *tbl,void *key){

    ch_table_entry_t *tbl_entry = NULL;

    if(tbl->last_entry&&_table_entry_equal(tbl,tbl->last_entry,key))
    {
        tbl_entry = tbl->last_entry;
        tbl->tbl_stats.hits_num += 1;
    }


    if(tbl_entry == NULL){
    
        tbl_entry = _table_entry_find_from_caches(tbl,key);
    }

    if(tbl_entry == NULL){
    
        tbl_entry = _table_entry_find(tbl,key);
    }

    if(tbl_entry){
    
        _entry_last_time_update(tbl_entry);
    }
    return tbl_entry;
}

ch_table_entry_t * ch_table_entry_find_create(ch_table_t *tbl,void *key){

	ch_table_entry_t *tbl_entry = NULL;

	tbl_entry = ch_table_entry_find(tbl,key);
	
	if(tbl_entry == NULL){
	
		tbl_entry = ch_table_entry_create(tbl,key);
	}

	return tbl_entry;
}

