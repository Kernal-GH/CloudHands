/*
 *
 *      Filename: ch_table.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-20 21:00:38
 * Last Modified: 2016-10-20 21:00:38
 */

#ifndef CH_TABLE_H
#define CH_TABLE_H

typedef struct ch_table_t ch_table_t;
typedef struct ch_table_entry_t ch_table_entry_t;
typedef struct ch_table_stat_t ch_table_stat_t;

#include <apr_pools.h>
#include "ch_util.h"
#include "ch_list.h"

struct ch_table_entry_t {

    struct ch_list_head entry;
    struct ch_list_head lru;
    uint64_t last_time;
};

struct ch_table_stat_t {

    /*the total numbers created*/
    uint32_t create_num;

    /*the total numbers freed*/
    uint32_t free_num;

    /*the total numbers cache hitted*/
    uint32_t hits_num;

    /*the total numbers cache no hitted*/
    uint32_t no_hits_num;

    /*the total numbers created failed*/
    uint32_t fail_num;

    /*the total numbers timeout*/
    uint32_t timeout_num;
};

struct ch_table_t {

    apr_pool_t *mp;
    ch_table_entry_t *last_entry;
    uint64_t entry_timeout;

    uint32_t entry_size;
    uint32_t n_entries;
    uint32_t n_entries_limit;

    uint32_t tbl_size;
    uint32_t tbl_mask;

    uint32_t n_caches;
    uint32_t n_caches_limits;

	uint64_t last_clean_time;

    void *priv_data;

    uint32_t (*entry_hash)(void *key,void *priv_data);
    int (*entry_equal)(ch_table_entry_t *entry,void *key,void *priv_data);
    void (*entry_clean)(ch_table_entry_t *entry,void *priv_data);
    ch_table_entry_t * (*entry_create)(void *key,void *priv_data);

    ch_table_stat_t tbl_stats;
    struct ch_list_head free_entries;
    struct ch_list_head caches;
    struct ch_list_head entries[0];
};

extern ch_table_t * ch_table_create(apr_pool_t *mp,
        uint32_t entry_size,uint32_t n_entries_limit,uint32_t tbl_size,
        uint64_t entry_timeout,uint32_t n_caches_limits,
        void *priv_data,
        uint32_t (*entry_hash)(void *key,void *priv_data),
        int (*entry_equal)(ch_table_entry_t *entry,void *key,void *priv_data),
        void (*entry_clean)(ch_table_entry_t *entry,void *priv_data),
		ch_table_entry_t * (*entry_create)(void *key,void *priv_data));

extern ch_table_entry_t * ch_table_entry_create(ch_table_t *tbl,void *key);


extern void  ch_table_entry_free(ch_table_t *tbl,ch_table_entry_t *entry);


extern ch_table_entry_t * ch_table_entry_find(ch_table_t *tbl,void *key);

extern ch_table_entry_t * ch_table_entry_find_create(ch_table_t *tbl,void *key);

extern size_t ch_table_entries_timeout_free(ch_table_t *tbl,
	int (*need_free)(ch_table_entry_t *entry,void *priv_data));

static inline void ch_table_dump(ch_table_t *tbl,char *b,size_t bsize){

	snprintf(b,bsize,"Table info:\n size:%lu\n entries:%lu\n createNum:%lu\n freeNum:%lu\n hitsNum:%lu\n noHitNum:%lu\n failNum:%lu\n timeoutNum:%lu\n",(unsigned long)tbl->entry_size,
		(unsigned long)tbl->n_entries,
		(unsigned long)tbl->tbl_stats.create_num,
		(unsigned long)tbl->tbl_stats.free_num,
		(unsigned long)tbl->tbl_stats.hits_num,
		(unsigned long)tbl->tbl_stats.no_hits_num,
		(unsigned long)tbl->tbl_stats.fail_num,
		(unsigned long)tbl->tbl_stats.timeout_num);
}

#endif /* CH_TABLE_H */
