/*
 * =====================================================================================
 *
 *       Filename:  ch_mpool.h
 *
 *    Description: a memory pool used to store fixed size objects,
 *    and free objects,or reuse threm
 *
 *        Version:  1.0
 *        Created:  2016年03月04日 16时43分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_MPOOL_H
#define CH_MPOOL_H

typedef struct ch_mpool_t ch_mpool_t;
typedef struct ch_mpool_entry_t ch_mpool_entry_t;
typedef struct ch_mpool_allocator_t ch_mpool_allocator_t;

#include "ch_list.h"

struct ch_mpool_t {

    /*the list header of all memory pool allocators*/
    ch_mpool_allocator_t *allocators;

    /*the list header of all entries that freed*/
    ch_mpool_entry_t *free_entries;

    /*the size of every allocator*/
    size_t allocator_size;

    /*the allocators number */
    size_t n_allocators;

    /*the free entries number*/
    size_t n_free_entries;

    /*the every entry size */
    uint16_t entry_size;

};

struct ch_mpool_allocator_t {

    /*link to next memory alloctor*/
    ch_mpool_allocator_t *next;

    /*the start address allocated for this memory allocator*/
    void *mbase;

    /*the start address that can be allocated*/
    void *mpos;

    /*the end address allocated for this memory allocator*/
    void *mend;

};

struct ch_mpool_entry_t {
    
    /*link to next entry*/
    ch_mpool_entry_t *next;

};

#define ch_mpool_entry_ptr(entry) ((ch_mpool_entry_t*)((void*)(entry)-sizeof(ch_mpool_entry_t)))

/*Create a memory pool
 * @mp,used to allocate the instance of ch_mpool_t
 * @allocator_size,the size of every allocator
 * @entry_size,the every entry size 
 * return: if ok,return the instance of ch_mpool_t,otherwise return NULL*/
extern ch_mpool_t *ch_mpool_create(apr_pool_t *mp,size_t allocator_size,uint16_t entry_size);


/*Destroy a memory pool*/
extern void ch_mpool_destroy(ch_mpool_t *mpool);


/*Reset memory pool,the mpool will back to the state created it*/
extern void ch_mpool_reset(ch_mpool_t *mpool);


/*Allocate an entry from mpool
 *@mpool,the instance of memory pool
 *@alloc_size,the size allocated
 *return: if ok,return the instance of entry,otherwise return NULL 
 */
extern void * ch_mpool_entry_alloc(ch_mpool_t *mpool,size_t *alloc_size);


/*Free an entry back to mpool,so that can be reused
 * @mpool,the instance of memory pool
 * @entry,the entry instance freed
 * @priv_data,user data ,can be used to call callback functions
 * */
extern void ch_mpool_entry_free(ch_mpool_t *mpool,void *entry);

#endif /*CH_MPOOL_H*/

