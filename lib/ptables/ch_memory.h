/*
 *
 *      Filename: ch_memory.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-27 09:28:59
 * Last Modified: 2017-12-27 09:28:59
 */

#ifndef CH_MEMORY_H
#define	CH_MEMORY_H

typedef struct ch_memory_item_t ch_memory_item_t;
typedef struct ch_memory_t ch_memory_t;

#include "ch_constants.h"
#include "ch_memory_cache.h"
#include "ch_mpool.h"
#include "ch_list.h"

enum {

	FROM_SYS,
	FROM_DPDK,
};

#define M_PROTECT_V 1234

struct ch_memory_item_t {
	int m_protect;
	struct list_head node;

	uint8_t type;
	uint8_t from_cache;
	size_t size;
	size_t rsize;
};

struct ch_memory_t {

	ch_pool_t *mp;
	ch_memory_cache_t *mcache;
	size_t use_size;
	size_t use_sys_size;
	size_t use_dpdk_size;

	size_t cache_hits_size; 

};


extern ch_memory_t* ch_memory_create(ch_pool_t *mp,size_t max_cache_size,uint64_t timeout);

extern void* ch_memory_alloc(ch_memory_t *mm,size_t size);

extern void* ch_memory_calloc(ch_memory_t *mm,size_t size);

extern void  ch_memory_free(ch_memory_t *mm,void *addr);

extern void ch_memory_item_free(ch_memory_t *mm,ch_memory_item_t *mitem);

extern void ch_memory_dump(ch_memory_t *mm,FILE *fp);

#endif /*CH_MEMORY_H*/
