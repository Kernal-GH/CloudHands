/*
 *
 *      Filename: ch_plist.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-13 10:52:21
 * Last Modified: 2018-09-13 10:52:21
 */

#ifndef CH_PLIST_H
#define CH_PLIST_H

typedef struct ch_plist_t ch_plist_t;
typedef struct ch_plist_entry_t ch_plist_entry_t;

#include "ch_mpool.h"
#include "ch_util.h"
#include "ch_list.h"
#include "ch_entry_pool.h"

struct ch_plist_entry_t {

	struct list_head node;
	uint64_t last_time;
};

struct ch_plist_t {

	struct list_head entries;

	ch_entry_pool_t *ep;

	ch_plist_entry_t *last_entry;

	size_t entry_size;
	size_t priv_data_size;
	void *priv_data;

	size_t entry_num;
	size_t entry_num_limits;


	uint64_t entry_timeout;
	uint64_t last_clean_time;

	int (*entry_equal)(ch_plist_entry_t *entry,void *key,void *priv_data);
	void (*entry_clean)(ch_plist_entry_t *entry,void *priv_data);
	void (*entry_timeout_cb)(ch_plist_entry_t *entry,uint64_t tv,void *priv_data);

};

extern ch_plist_t * ch_plist_create(ch_pool_t *mp,int pool_type,
        size_t entry_size,size_t priv_data_size,size_t entry_num_limits,
        uint64_t entry_timeout,
        void *priv_data,
        int (*entry_equal)(ch_plist_entry_t *entry,void *key,void *priv_data),
        void (*entry_clean)(ch_plist_entry_t *entry,void *priv_data),
		void (*entry_timeout_cb)(ch_plist_entry_t *entry,uint64_t tv,void *priv_data));

extern ch_plist_entry_t * ch_plist_entry_create(ch_plist_t *plist);

extern void  ch_plist_entry_free(ch_plist_t *plist,ch_plist_entry_t *entry);

extern ch_plist_entry_t * ch_plist_entry_find(ch_plist_t *plist,void *key);

extern ch_plist_entry_t * ch_plist_entry_find_create(ch_plist_t *plist,void *key);

extern size_t ch_plist_entries_timeout_free(ch_plist_t *plist,
	int (*need_free)(ch_plist_entry_t *entry,void *priv_data));

extern void ch_plist_dump(ch_plist_t *plist,FILE *fp);

static inline void * ch_plist_entry_priv_data(ch_plist_t *plist,void *entry){

	if(entry == NULL || plist->priv_data_size == 0)
		return NULL;

	return entry+plist->entry_size;
}


#endif /*CH_PLIST_H*/
