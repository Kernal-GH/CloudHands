/*
 *
 *      Filename: ch_mpool.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 10:07:10
 * Last Modified: 2017-01-09 10:07:10
 */

#ifndef CH_MPOOL_H
#define CH_MPOOL_H

typedef void (*ch_pool_cleanup_pt)(void *data);
typedef struct ch_pool_cleanup_t  ch_pool_cleanup_t;
typedef struct ch_pool_large_t ch_pool_large_t;
typedef struct ch_pool_t ch_pool_t;
typedef struct ch_pool_data_t ch_pool_data_t;

#include "ch_constants.h"
#include "ch_list.h"

#define CH_MAX_ALLOC_FROM_POOL  (4096 - 1)

#define CH_DEFAULT_POOL_SIZE    (16 * 1024)

#define CH_POOL_ALIGNMENT       16
#define CH_MIN_POOL_SIZE                                                     \
    ch_align((sizeof(ch_pool_t) + 2 * sizeof(ch_pool_large_t)),            \
              CH_POOL_ALIGNMENT)



struct ch_pool_cleanup_t {
    ch_pool_cleanup_pt   handler;
    void                 *data;
    ch_pool_cleanup_t   *next;
};


struct ch_pool_large_t {
    ch_pool_large_t     *next;
    void                 *alloc;
};


struct ch_pool_data_t {
    void               *last;
    void               *end;
    ch_pool_t           *next;
    size_t           failed;
};


struct ch_pool_t {

    //struct list_head node;
    ch_pool_data_t       d;
    size_t                max;
    ch_pool_t           *current;
    ch_pool_large_t     *large;
    ch_pool_cleanup_t   *cleanup;
};

extern ch_pool_t *
ch_pool_create(size_t size);

extern void 
ch_pool_destroy(ch_pool_t *pool);

extern void 
ch_pool_reset(ch_pool_t *pool);

extern void *
ch_palloc(ch_pool_t *pool, size_t size);

extern void *
ch_pnalloc(ch_pool_t *pool, size_t size);

extern void *
ch_pcalloc(ch_pool_t *pool, size_t size);

extern void *
ch_pmemalign(ch_pool_t *pool, size_t size, size_t alignment);

extern int 
ch_pfree(ch_pool_t *pool, void *p);


extern ch_pool_cleanup_t *
ch_pool_cleanup_add(ch_pool_t *p, size_t size);

#endif /* CH_MPOOL_H */
