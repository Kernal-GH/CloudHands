/*
 *
 *      Filename: ch_ptable.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-20 21:00:38
 * Last Modified: 2016-10-20 21:00:38
 */

#ifndef CH_PTABLE_H
#define CH_PTABLE_H

typedef struct ch_ptable_t ch_ptable_t;
typedef struct ch_ptable_entry_t ch_ptable_entry_t;
typedef struct ch_ptable_stat_t ch_ptable_stat_t;

#include "ch_mpool.h"
#include "ch_util.h"
#include "ch_list.h"
#include "ch_entry_pool.h"

struct ch_ptable_entry_t {

    struct list_head entry;
    struct list_head lru;
    uint64_t last_time;
};

struct ch_ptable_stat_t {

    /*the total numbers cache hitted*/
    size_t hits_num;

    /*the total numbers cache no hitted*/
    size_t no_hits_num;

    /*the total numbers created failed*/
    size_t fail_num;

    /*the total numbers timeout*/
    size_t timeout_num;
};

struct ch_ptable_t {

	ch_entry_pool_t *ep;

	ch_ptable_entry_t *last_entry;
    uint64_t entry_timeout;

	size_t entry_size;
	size_t priv_data_size;

    size_t tbl_size;
    size_t tbl_mask;

    size_t n_caches;
    size_t n_caches_limits;

	uint64_t last_clean_time;

    void *priv_data;

    size_t (*entry_hash)(void *key,void *priv_data);
    int (*entry_equal)(ch_ptable_entry_t *entry,void *key,void *priv_data);
    void (*entry_clean)(ch_ptable_entry_t *entry,void *priv_data);
	void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data);

    ch_ptable_stat_t tbl_stats;
    struct list_head caches;
    struct list_head entries[0];
};

extern ch_ptable_t * ch_ptable_create(ch_pool_t *mp,int pool_type,
        size_t entry_size,size_t priv_data_size,size_t n_entries_limit,size_t tbl_size,
        uint64_t entry_timeout,size_t n_caches_limits,
        void *priv_data,
        size_t (*entry_hash)(void *key,void *priv_data),
        int (*entry_equal)(ch_ptable_entry_t *entry,void *key,void *priv_data),
        void (*entry_clean)(ch_ptable_entry_t *entry,void *priv_data),
		void (*entry_timeout_cb)(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data));

extern ch_ptable_entry_t * ch_ptable_entry_create(ch_ptable_t *tbl,void *key);


extern ch_ptable_entry_t * ch_ptable_entry_create_with_data(ch_ptable_t *tbl,void *key,void *priv_data);


extern void  ch_ptable_entry_free(ch_ptable_t *tbl,ch_ptable_entry_t *entry);


extern ch_ptable_entry_t * ch_ptable_entry_find(ch_ptable_t *tbl,void *key);

extern ch_ptable_entry_t * ch_ptable_entry_find_create(ch_ptable_t *tbl,void *key);

extern size_t ch_ptable_entries_timeout_free(ch_ptable_t *tbl,
	int (*need_free)(ch_ptable_entry_t *entry,void *priv_data));

extern void ch_ptable_dump(ch_ptable_t *tbl,FILE *fp);

static inline void * ch_ptable_entry_priv_data(ch_ptable_t *tbl,void *entry){

	if(entry == NULL || tbl->priv_data_size == 0)
		return NULL;

	return entry+tbl->entry_size;
}

#endif /* CH_PTABLE_H */
