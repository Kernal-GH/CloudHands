/*
 *
 *      Filename: ch_entry_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-27 12:00:28
 * Last Modified: 2017-12-27 12:00:28
 */

#ifndef CH_ENTRY_POOL_H
#define CH_ENTRY_POOL_H

typedef struct ch_entry_pool_t ch_entry_pool_t;

#include "ch_object_pool.h"
#include "ch_mpool.h"
#include <rte_mempool.h>

enum {

	FROM_DPDK_POOL,
	FROM_OBJ_POOL
};

struct ch_entry_pool_t {

	union {
	
		ch_object_pool_t  *sys_mp;
		struct rte_mempool *dpdk_mp;
	};

	int type;

	size_t entry_size;
	size_t n_entries_limit;

};

extern ch_entry_pool_t * ch_entry_pool_create(ch_pool_t *mp,
	int type,
	size_t entry_size,
	size_t n_entries_limit,
	size_t n_caches_limit);


extern void ch_entry_pool_destroy(ch_entry_pool_t *ep);

extern void * ch_entry_pool_alloc(ch_entry_pool_t *ep);

extern void ch_entry_pool_free(ch_entry_pool_t *ep,void *pentry);

extern void ch_entry_pool_dump(ch_entry_pool_t *ep,FILE *fp);

#endif /*CH_ENTRY_POOL_H*/
