/*
 * =====================================================================================
 *
 *       Filename:  ch_mpool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年03月04日 18时58分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_constants.h"
#include "ch_log.h"
#include "ch_mpool.h"
#include "ch_slist.h"

ch_mpool_t *ch_mpool_create(apr_pool_t *mp,size_t allocator_size,uint16_t entry_size)
{
    ch_mpool_t *mpool = NULL;

    size_t page_size,malloc_size_align,entry_size_align;

    if(allocator_size == 0|| entry_size == 0 || entry_size>=allocator_size ){
        ch_log(CH_LOG_ERR,"Invalid args!");
        return NULL;
    }

    page_size = sysconf(_SC_PAGE_SIZE);
    malloc_size_align = ch_align_up(allocator_size+sizeof(ch_mpool_allocator_t),page_size);
    entry_size_align = ch_align_up(entry_size+sizeof(ch_mpool_entry_t),sizeof(ch_mpool_entry_t));

    mpool = (ch_mpool_t*)apr_palloc(mp,sizeof(ch_mpool_t));
    mpool->allocators = NULL;
    mpool->free_entries = NULL;

    mpool->allocator_size = malloc_size_align;
    mpool->n_allocators = 0;
    mpool->n_free_entries = 0;
    mpool->entry_size = entry_size_align;
    return mpool;
}

static ch_mpool_allocator_t * _mp_allocator_create(ch_mpool_t *mpool){

    ch_mpool_allocator_t * mp_alloc = NULL;
    
    mp_alloc = (ch_mpool_allocator_t*)malloc(mpool->allocator_size);
    if(!mp_alloc){
        ch_log(CH_LOG_ERR,"No memory used to create memory pool allocator!");
        return NULL;
    }

    mp_alloc->next = NULL;
    mp_alloc->mbase = (void*)(mp_alloc+1);
    mp_alloc->mpos = mp_alloc->mbase;
    mp_alloc->mend = ((void*)mp_alloc)+mpool->allocator_size;

    /*add it to memory pool*/
    ch_slist_entry_add(mpool->allocators,mp_alloc,next);
    mpool->n_allocators +=1;

    return mp_alloc;
}

void ch_mpool_reset(ch_mpool_t *mpool){

    ch_mpool_allocator_t *mp_alloc,*mp_alloc_tmp;

    ch_slist_foreach_entry_safe(mpool->allocators,mp_alloc,mp_alloc_tmp,next){
        
        free(mp_alloc);
    }

    mpool->allocators = NULL;
    mpool->free_entries = NULL;

    mpool->n_allocators = 0;
    mpool->n_free_entries = 0;

}

void ch_mpool_destroy(ch_mpool_t *mpool){
    
    ch_mpool_reset(mpool);
}

void * ch_mpool_entry_alloc(ch_mpool_t *mpool,size_t *alloc_size){

    ch_mpool_entry_t *mp_entry = NULL;
    void *obj = NULL;
    ch_mpool_allocator_t *mp_alloc = NULL;
    *alloc_size = (size_t)(mpool->entry_size-sizeof(ch_mpool_entry_t));

    /*if memory pool has entry freed,then allocat it */
    if(mpool->free_entries){
        mp_entry = mpool->free_entries;
        mpool->free_entries = mp_entry->next;
        mp_entry->next = NULL;

        obj = (void*)(mp_entry+1);

        mpool->n_free_entries-=1;
        return obj;
    }

    mp_alloc = mpool->allocators;
    if(mp_alloc == NULL || (mp_alloc->mpos+mpool->entry_size>mp_alloc->mend)){
        /*need allocate a new allocator */
        mp_alloc = _mp_allocator_create(mpool);

    }

    if(mp_alloc == NULL){
        ch_log(CH_LOG_ERR,"No memory space used to allocate a new entry!");
        return NULL;
    }

    /*Allocate a new memory entry from this allocator*/
    mp_entry = (ch_mpool_entry_t*)mp_alloc->mpos;
    mp_entry->next = NULL;

    obj = (void*)(mp_entry+1);

    mp_alloc->mpos = mp_alloc->mpos+mpool->entry_size;

    /*ok*/

    return obj;
}

void ch_mpool_entry_free(ch_mpool_t *mpool,void *entry){
    
    ch_mpool_entry_t *mp_entry;

    mp_entry = ch_mpool_entry_ptr(entry);

    mp_entry->next = NULL;

    ch_slist_entry_add(mpool->free_entries,mp_entry,next);
    mpool->n_free_entries+=1;

}
