/*
 *
 *      Filename: ch_ptable.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-20 22:32:06
 * Last Modified: 2018-09-13 11:55:06
 */

#include "ch_log.h"
#include "ch_ptable.h"

static inline int _tbl_size_check(size_t tbl_size,size_t n_entries_limit){

    if(tbl_size == 0){
    
        ch_log(CH_LOG_ERR," ptable size must >0");
        return 1;
    }

    if(!ch_is_power_of_2(tbl_size)){
    
        ch_log(CH_LOG_ERR," ptable size must 2 power!");
        return 1;
    }

    if(tbl_size>n_entries_limit){
    
        ch_log(CH_LOG_ERR," ptable size too large!");
        return 1;
    }

    return 0;
}

static inline void _ptable_stats_init(ch_ptable_stat_t *stat){

    stat->hits_num = 0;
    stat->no_hits_num = 0;
    stat->fail_num = 0;
    stat->timeout_num = 0;
}

static inline struct list_head * entry_header(ch_ptable_t *tbl,size_t idx){

    return tbl->entries+idx;
}

static inline void _ptable_entries_init(ch_ptable_t *tbl,size_t tbl_size){

    size_t i ;
    struct list_head *h;

    for(i = 0; i<tbl_size; i++){
    
        h = entry_header(tbl,i);
        INIT_LIST_HEAD(h);
    }
}


ch_ptable_t * ch_ptable_create(ch_pool_t *mp,int pool_type,
        size_t entry_size,size_t priv_data_size,size_t n_entries_limit,size_t tbl_size,
        uint64_t entry_timeout,size_t n_caches_limits,
        void *priv_data,
        size_t (*entry_hash)(void *key,void *priv_data),
        int (*entry_equal)(ch_ptable_entry_t *entry,void *key,void *priv_data),
        void (*entry_clean)(ch_ptable_entry_t *entry,void *priv_data),
		void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data)){

    size_t r_tbl_size,tbl_mask;
    size_t msize = 0;

    ch_ptable_t *tbl = NULL;

    r_tbl_size = (size_t)ch_align64pow2((uint64_t)tbl_size);

    /*check failed!*/
    if(_tbl_size_check(r_tbl_size,n_entries_limit))
        return NULL;
	

    
	tbl_mask = r_tbl_size -1;
    msize = sizeof(*tbl)+r_tbl_size*sizeof(tbl->entries[0]);

    tbl = (ch_ptable_t*)ch_palloc(mp,msize);
    if(tbl == NULL){
        ch_log(CH_LOG_ERR,"No space used to create assemble ptable!");
        return NULL;
    }
	
	tbl->ep = ch_entry_pool_create(mp,pool_type,entry_size+priv_data_size,n_entries_limit,n_caches_limits);
	if(tbl->ep == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create entry pool for ptable!");
		return NULL;
	}


    tbl->last_entry = NULL;
    tbl->entry_timeout = entry_timeout;
	tbl->entry_size = entry_size;
	tbl->priv_data_size = priv_data_size;

    tbl->tbl_size = r_tbl_size;
    tbl->tbl_mask = tbl_mask;
    tbl->n_caches = 0;
    tbl->n_caches_limits = n_caches_limits;
    tbl->priv_data = priv_data;
    tbl->entry_hash = entry_hash;
    tbl->entry_equal = entry_equal;
    tbl->entry_clean = entry_clean;
	tbl->entry_timeout_cb = entry_timeout_cb;

	tbl->last_clean_time = ch_get_current_timems()/1000;

    _ptable_stats_init(&tbl->tbl_stats);
    INIT_LIST_HEAD(&tbl->caches);
    _ptable_entries_init(tbl,r_tbl_size);


    return tbl;
}

static inline void lru_node_init(struct list_head *node){

    node->prev = NULL;
    node->next = NULL;
}

static inline int lru_node_empty(struct list_head *node){

    return node->prev == NULL && node->next == NULL;
}

static inline void tbl_entry_free(ch_ptable_t *tbl,ch_ptable_entry_t *tbl_entry,uint64_t is_timeout){

    list_del(&tbl_entry->entry);
    
    if(!lru_node_empty(&tbl_entry->lru)){
        list_del(&tbl_entry->lru);
        tbl->n_caches-=1;
    }

    if(tbl_entry == tbl->last_entry)
        tbl->last_entry = NULL;

	if(is_timeout){
		tbl->tbl_stats.timeout_num +=1;
		if(tbl->entry_timeout_cb){
		
			tbl->entry_timeout_cb(tbl_entry,is_timeout,tbl->priv_data);
		}
	}
	if(tbl->entry_clean)
		tbl->entry_clean(tbl_entry,tbl->priv_data);

	ch_entry_pool_free(tbl->ep,tbl_entry);

}


static inline void _entry_last_time_update(ch_ptable_entry_t *tbl_entry){

    tbl_entry->last_time = (uint64_t)(ch_get_current_timems()/1000); 
}

static inline uint64_t  _entry_is_timeout(ch_ptable_t *tbl,ch_ptable_entry_t *tbl_entry){

    uint64_t cur_time = (uint64_t)(ch_get_current_timems()/1000);
	uint64_t tv = cur_time-tbl_entry->last_time;

    if(tv>tbl->entry_timeout)
        return tv;

    return 0;
}

/*free entries timeout */
size_t ch_ptable_entries_timeout_free(ch_ptable_t *tbl,
	int (*need_free)(ch_ptable_entry_t *entry,void *priv_data)){

	size_t c = 0;

    struct list_head *h;
    size_t i;
    uint64_t cur_time = ch_get_current_timems()/1000; 
	uint64_t tv;

	ch_ptable_entry_t *entry,*tmp_entry;

	if((cur_time-tbl->last_clean_time)<tbl->entry_timeout)
		return 0;

	tbl->last_clean_time = cur_time;

	for(i = 0; i<tbl->tbl_size; i++){
    
        h = entry_header(tbl,i);
        
        list_for_each_entry_safe(entry,tmp_entry,h,entry){

			tv = _entry_is_timeout(tbl,entry);

            if(tv||(need_free&&need_free(entry,tbl->priv_data))){
				c+=1;
                tbl_entry_free(tbl,entry,tv);
            }
        }
    }


	return c;
}

void _entries_timeout_free(ch_ptable_t *tbl){

    struct list_head *h;
    size_t i;
	uint64_t tv;
    ch_ptable_entry_t *entry,*tmp_entry;

	for(i = 0; i<tbl->tbl_size; i++){
    
        h = entry_header(tbl,i);
        
        list_for_each_entry_safe(entry,tmp_entry,h,entry){
       
			tv = _entry_is_timeout(tbl,entry);

            if(tv){
                tbl_entry_free(tbl,entry,tv);
            }
        }
    }

}


static inline struct list_head* _ptable_header_get(ch_ptable_t *tbl,void *key,void *priv_data){

    size_t hash = tbl->entry_hash(key,priv_data);

    return entry_header(tbl,hash&tbl->tbl_mask);
}

static inline void _ptable_insert(ch_ptable_t *tbl,ch_ptable_entry_t *tbl_entry,
        void *key,void *priv_data){

    struct list_head *h = _ptable_header_get(tbl,key,priv_data);

    /*insert into header*/
    list_add(&tbl_entry->entry,h);

}

static inline void _ptable_caches_insert(ch_ptable_t *tbl,ch_ptable_entry_t *tbl_entry){

    ch_ptable_entry_t *tmp_entry;
    if(tbl->n_caches>= tbl->n_caches_limits){
    
        tmp_entry = list_last_entry(&tbl->caches,ch_ptable_entry_t,lru);
        list_del(&tmp_entry->lru);
        lru_node_init(&tmp_entry->lru);

    }else{
    
        tbl->n_caches+=1;
    }

    list_add(&tbl_entry->lru,&tbl->caches);

}

static void _ptable_entry_init(ch_ptable_t *tbl,ch_ptable_entry_t *tbl_entry,
        void *key,void *priv_data){

   
    _entry_last_time_update(tbl_entry);

    tbl->last_entry = tbl_entry;

    _ptable_insert(tbl,tbl_entry,key,priv_data);
    _ptable_caches_insert(tbl,tbl_entry);
}

ch_ptable_entry_t * ch_ptable_entry_create(ch_ptable_t *tbl,void *key){

    ch_ptable_entry_t *tbl_entry = NULL;

	tbl_entry = ch_entry_pool_alloc(tbl->ep);

    if(tbl_entry == NULL){
    
        ch_log(CH_LOG_WARN,
			"Cannot alloc new table entry ,try to free some timeout entries and try again --------------");

        _entries_timeout_free(tbl);
	
		tbl_entry = ch_entry_pool_alloc(tbl->ep);

        if(tbl_entry == NULL){
       
            tbl->tbl_stats.fail_num +=1;
            ch_log(CH_LOG_ERR,"The number of entries allocated has been overflow,no entries timeout are freed!");
    
            return NULL;
        }
    }
	
	_ptable_entry_init(tbl,tbl_entry,key,tbl->priv_data);


    return tbl_entry;
}

ch_ptable_entry_t * ch_ptable_entry_create_with_data(ch_ptable_t *tbl,void *key,void *priv_data){

    ch_ptable_entry_t *tbl_entry = NULL;

	tbl_entry = ch_entry_pool_alloc(tbl->ep);

    if(tbl_entry == NULL){
    
        ch_log(CH_LOG_WARN,
			"Cannot alloc new table entry ,try to free some timeout entries and try again --------------");

        _entries_timeout_free(tbl);
	
		tbl_entry = ch_entry_pool_alloc(tbl->ep);

        if(tbl_entry == NULL){
       
            tbl->tbl_stats.fail_num +=1;
            ch_log(CH_LOG_ERR,"The number of entries allocated has been overflow,no entries timeout are freed!");
    
            return NULL;
        }
    }
	
	_ptable_entry_init(tbl,tbl_entry,key,priv_data);


    return tbl_entry;
}

void ch_ptable_entry_free(ch_ptable_t *tbl,ch_ptable_entry_t *entry){
 
    tbl_entry_free(tbl,entry,0);
}

static inline int _ptable_entry_equal(ch_ptable_t *tbl,ch_ptable_entry_t *tbl_entry,
        void *key){

    return tbl->entry_equal(tbl_entry,key,tbl->priv_data);
}

static ch_ptable_entry_t * _ptable_entry_find_from_caches(ch_ptable_t *tbl,void *key){

    ch_ptable_entry_t *tbl_entry = NULL;
    ch_ptable_entry_t *tbl_entry_fist = NULL;

    int found = 0;
    struct list_head *h = &tbl->caches;

    list_for_each_entry(tbl_entry,h,lru){
    
        if(_ptable_entry_equal(tbl,tbl_entry,key))
        {
        
            found = 1;
            break;
        }

    }

    if(found){
    
        tbl->tbl_stats.hits_num +=1;
        tbl_entry_fist = list_first_entry(h,ch_ptable_entry_t,lru);
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

static inline ch_ptable_entry_t * _ptable_entry_find(ch_ptable_t *tbl,void *key){

    struct list_head *h = _ptable_header_get(tbl,key,tbl->priv_data);

    ch_ptable_entry_t *tbl_entry = NULL;
    int found = 0;

    list_for_each_entry(tbl_entry,h,entry){
    
        if(_ptable_entry_equal(tbl,tbl_entry,key)){

            found = 1;
            break;
        }
    }

    if(found){
    
        _ptable_caches_insert(tbl,tbl_entry);
        tbl->last_entry = tbl_entry;
        tbl->tbl_stats.no_hits_num += 1;
    }else{
    
        tbl_entry = NULL;
    }

    return tbl_entry;
}

ch_ptable_entry_t * ch_ptable_entry_find(ch_ptable_t *tbl,void *key){

    ch_ptable_entry_t *tbl_entry = NULL;

    if(tbl->last_entry&&_ptable_entry_equal(tbl,tbl->last_entry,key))
    {
        tbl_entry = tbl->last_entry;
        tbl->tbl_stats.hits_num += 1;
    }


    if(tbl_entry == NULL){
    
        tbl_entry = _ptable_entry_find_from_caches(tbl,key);
    }

    if(tbl_entry == NULL){
    
        tbl_entry = _ptable_entry_find(tbl,key);
    }

    if(tbl_entry){
    
        _entry_last_time_update(tbl_entry);
    }
    return tbl_entry;
}

ch_ptable_entry_t * ch_ptable_entry_find_create(ch_ptable_t *tbl,void *key){

	ch_ptable_entry_t *tbl_entry = NULL;

	tbl_entry = ch_ptable_entry_find(tbl,key);
	
	if(tbl_entry == NULL){
	
		tbl_entry = ch_ptable_entry_create(tbl,key);
	}

	return tbl_entry;
}

void ch_ptable_dump(ch_ptable_t *tbl,FILE *fp){

	fprintf(fp,"ptable info:hitsNum:%lu\n noHitNum:%lu\n failNum:%lu\n timeoutNum:%lu\n",
		(unsigned long)tbl->tbl_stats.hits_num,
		(unsigned long)tbl->tbl_stats.no_hits_num,
		(unsigned long)tbl->tbl_stats.fail_num,
		(unsigned long)tbl->tbl_stats.timeout_num);

	ch_entry_pool_dump(tbl->ep,fp);
}
