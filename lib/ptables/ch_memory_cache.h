/*
 *
 *      Filename: ch_memory_cache.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-03 11:02:45
 * Last Modified: 2018-01-03 11:02:45
 */

#ifndef CH_MEMORY_CACHE_H
#define CH_MEMORY_CACHE_H

typedef struct ch_memory_cache_t ch_memory_cache_t;
typedef struct ch_memory_cache_header_t ch_memory_cache_header_t;

#include "ch_memory.h"
#include "ch_list.h"

#define MAX_CACHE_POW 16
#define MAX_CACHE_MITEM_SIZE 65536
#define MAX_CACHE_SIZE_D 67108864 
#define MAX_CACHE_INDEX MAX_CACHE_POW+1

struct ch_memory_cache_header_t {

	size_t n_hits;
	size_t n_nohits;
	size_t n_caches;
	uint64_t last_find_time;

	struct list_head items;
};

struct ch_memory_cache_t {

	size_t max_cache_size;
	size_t cur_cache_size;

	uint64_t timeout;

	ch_memory_t *mm;

	ch_memory_cache_header_t cache_headers[MAX_CACHE_INDEX];
};


extern ch_memory_cache_t* ch_memory_cache_create(ch_memory_t *mm,size_t max_cache_size,uint64_t timeout);

extern void ch_memory_cache_reset(ch_memory_cache_t *mc);

extern ch_memory_item_t * ch_memory_cache_find(ch_memory_cache_t *mc,size_t size);

extern int ch_memory_cache_update(ch_memory_cache_t *mc,ch_memory_item_t *mitem);

extern void ch_memory_cache_dump(ch_memory_cache_t *mc,FILE *fp);

#endif /*CH_MEMORY_CACHE_H*/
