/*
 * =====================================================================================
 *
 *       Filename:  ch_hash_pool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月12日 18时15分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_log.h"
#include "ch_hash_pool.h"


ch_hash_pool_t * ch_hash_pool_create(apr_pool_t *mp,size_t entry_size,size_t n_entries_limit,void *priv_data,
        uint32_t (*entry_hash)(ch_four_tuple_t *tuple,void *priv_data),
        int (*entry_equal)(ch_hash_entry_t *entry,ch_four_tuple_t *tuple,void *priv_data)
        ){
    int i;
    ch_hash_pool_t *hpool = (ch_hash_pool_t*)apr_palloc(mp,sizeof(ch_hash_pool_t));

    hpool->mp = mp;
    hpool->entry_size = entry_size;
    hpool->n_entries = 0;
    hpool->n_entries_limit = 0;
    hpool->priv_data = priv_data;
    hpool->entry_hash = entry_hash;
    hpool->entry_equal = entry_equal;

    CH_INIT_LIST_HEAD(&hpool->free_entries);

    for(i = 0; i<N_HASH_POOL_SIZE;i++){
        CH_INIT_LIST_HEAD(&hpool->entries[i]);
    }

    return hpool;
}


static inline struct ch_list_head * _hash_pool_header_get(ch_hash_pool_t *hpool,ch_four_tuple_t *tuple){

    uint32_t hash = hpool->entry_hash(tuple,hpool->priv_data);

    return &hpool->entries[hash&N_HASH_POOL_SIZE_MASK];
}

static inline int _hash_entry_equal(ch_hash_pool_t *hpool,ch_hash_entry_t *entry,ch_four_tuple_t *tuple){
    
    return hpool->entry_equal(entry,tuple,hpool->priv_data);
}

static inline void _hash_entry_insert(ch_hash_pool_t *hpool,ch_hash_entry_t *entry,ch_four_tuple_t *tuple){
   
    struct ch_list_head *header = _hash_pool_header_get(hpool,tuple);

    /*insert into header*/
    list_add(&entry->node,header);
}

ch_hash_entry_t * ch_hash_pool_entry_create(ch_hash_pool_t *hpool,ch_four_tuple_t *tuple){
    
    ch_hash_entry_t *entry = NULL;

    /* Try to get a free entry from free entries list */
    if(!list_empty(&hpool->free_entries)){
       
        entry = list_entry(&hpool->free_entries,ch_hash_entry_t,node);
        list_del(&entry->node);

    }else{
        if(hpool->n_entries_limit&&hpool->n_entries_limit<hpool->n_entries+1){

            ch_log(CH_LOG_ERR,"The number of entries created is overflow!");
            return NULL;
        }

        entry = (ch_hash_entry_t*)apr_palloc(hpool->mp,hpool->entry_size);
        if(entry == NULL){
            ch_log(CH_LOG_ERR,"No memory used to create a new hash entry!");
            return NULL;
        }

        hpool->n_entries+=1;
    }

    _hash_entry_insert(hpool,entry,tuple);

    return entry;
}


void ch_hash_pool_entry_free(ch_hash_pool_t *hpool,ch_hash_entry_t *entry){
    
    /*unlink from entries list */
    list_del(&entry->node);

    list_add(&entry->node,&hpool->free_entries);
    hpool->n_entries-=1;
}


ch_hash_entry_t * ch_hash_pool_entry_find(ch_hash_pool_t *hpool,ch_four_tuple_t *tuple){
    
    struct ch_list_head *header = _hash_pool_header_get(hpool,tuple);
    ch_hash_entry_t *entry = NULL;

    list_for_each_entry(entry,header,node){
        
        if(_hash_entry_equal(hpool,entry,tuple)){
            /*found*/
            return entry;
        }
    }

    /*no found*/
    return NULL;
}

