/*
 * =====================================================================================
 *
 *       Filename:  ch_hash_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月12日 17时45分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_HASH_POOL_H
#define CH_HASH_POOL_H

typedef struct ch_hash_pool_t ch_hash_pool_t;
typedef struct ch_hash_entry_t ch_hash_entry_t;

#include <apr_pools.h>
#include "ch_util.h"
#include "ch_list.h"

#define N_HASH_POOL_SIZE  65536
#define N_HASH_POOL_SIZE_MASK 65535

struct ch_hash_entry_t {

    struct ch_list_head node;
};

struct ch_hash_pool_t {

    struct ch_list_head entries[N_HASH_POOL_SIZE];
    struct ch_list_head free_entries;

    apr_pool_t *mp;
    
    size_t entry_size;
    size_t n_entries;
    size_t n_entries_limit;

    void *priv_data;

    uint32_t (*entry_hash)(ch_four_tuple_t *tuple,void *priv_data);
    int (*entry_equal)(ch_hash_entry_t *entry,ch_four_tuple_t *tuple,void *priv_data);

};

extern ch_hash_pool_t * ch_hash_pool_create(apr_pool_t *mp,size_t entry_size,size_t n_entries_limit,void *priv_data,
        uint32_t (*entry_hash)(ch_four_tuple_t *tuple,void *priv_data),
        int (*entry_equal)(ch_hash_entry_t *entry,ch_four_tuple_t *tuple,void *priv_data)
        );


extern ch_hash_entry_t * ch_hash_pool_entry_create(ch_hash_pool_t *hpool,ch_four_tuple_t *tuple);


extern void  ch_hash_pool_entry_free(ch_hash_pool_t *hpool,ch_hash_entry_t *entry);


extern ch_hash_entry_t * ch_hash_pool_entry_find(ch_hash_pool_t *hpool,ch_four_tuple_t *tuple);

#endif /* CH_HASH_POOL_H */

