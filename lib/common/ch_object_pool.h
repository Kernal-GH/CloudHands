/*
 *
 *      Filename: ch_object_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2017-12-27 13:52:28
 * Last Modified: 2017-12-27 13:52:28
 */

#ifndef CH_OBJECT_POOL_H
#define CH_OBJECT_POOL_H

typedef struct ch_object_pool_t ch_object_pool_t;
typedef struct ch_object_mem_t ch_object_mem_t;
typedef struct ch_object_item_t ch_object_item_t;

#include "ch_mpool.h"
#include "ch_list.h"

#define OBJECT_MEM_SIZE 64*1024

struct ch_object_pool_t {

	ch_object_mem_t *mm_list;

	struct list_head free_list;

	void *priv_data;

	void (*obj_init)(void *obj,void *priv_data);

	size_t object_limits;
	size_t object_size;

	size_t n_objects;

	size_t n_mms;

	size_t n_frees;
	
	size_t n_using;
};

struct ch_object_mem_t {

	ch_object_mem_t *next;
};

struct ch_object_item_t {

	struct list_head node;
};


extern ch_object_pool_t * ch_object_pool_create(ch_pool_t *mp,size_t object_limits,
	size_t object_size,
	void *priv_data,
	void (*obj_init)(void *obj,void *priv_data));

extern void ch_object_pool_destroy(ch_object_pool_t *omp);

extern void * ch_object_pool_get(ch_object_pool_t *omp);

extern void  ch_object_pool_put(ch_object_pool_t *omp,void *obj);

extern void ch_object_pool_dump(ch_object_pool_t *omp,FILE *fp);

#endif /*CH_OBJECT_POOL_H*/
