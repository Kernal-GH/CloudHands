/*
 *
 *      Filename: ch_mpool.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2017-01-09 11:04:58
 * Last Modified: 2018-05-04 12:08:59
 */

#include "ch_mpool.h"

ch_pool_t *
ch_pool_create(size_t size)
{
    ch_pool_t  *p;

    p = (ch_pool_t*)memalign(CH_POOL_ALIGNMENT, size);

    if (p == NULL) {
        return NULL;
    }

    p->d.last = (void *) p + sizeof(ch_pool_t);
    p->d.end = (void *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(ch_pool_t);
    p->max = (size < CH_MAX_ALLOC_FROM_POOL) ? size : CH_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->large = NULL;
    p->cleanup = NULL;

    return p;
}

void
ch_pool_destroy(ch_pool_t *pool)
{
    ch_pool_t          *p, *n;
    ch_pool_large_t    *l;
    ch_pool_cleanup_t  *c;

    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            c->handler(c->data);
        }
    }

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        free(p);

        if (n == NULL) {
            break;
        }
    }
}


void
ch_pool_reset(ch_pool_t *pool)
{
    ch_pool_t        *p;
    ch_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }

    for (p = pool; p; p = p->d.next) {
        p->d.last = (void *) p + sizeof(ch_pool_t);
        p->d.failed = 0;
    }

    pool->current = pool;
    pool->large = NULL;
}

static void *
ch_palloc_block(ch_pool_t *pool, size_t size)
{
    void      *m;
    size_t       psize;
    ch_pool_t  *p, *new;

    psize = (size_t) (pool->d.end - (void *) pool);

    m = memalign(CH_POOL_ALIGNMENT, psize);

    if (m == NULL) {
        return NULL;
    }

    new = (ch_pool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(ch_pool_data_t);
    m = ch_align_ptr(m, CH_ALIGNMENT);
    new->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            pool->current = p->d.next;
        }
    }

    p->d.next = new;

    return m;
}

static ch_inline void *
ch_palloc_small(ch_pool_t *pool, size_t size, unsigned int align)
{
    void      *m;
    ch_pool_t  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = ch_align_ptr(m, CH_ALIGNMENT);
        }

        if ((size_t) (p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }

        p = p->d.next;

    } while (p);

    return ch_palloc_block(pool, size);
}




static void *
ch_palloc_large(ch_pool_t *pool, size_t size)
{
    void              *p;
    unsigned int         n;
    ch_pool_large_t  *large;

    p = malloc(size);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = ch_palloc_small(pool, sizeof(ch_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

void *
ch_palloc(ch_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return ch_palloc_small(pool, size, 1);
    }

    return ch_palloc_large(pool, size);
}


void *
ch_pnalloc(ch_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        return ch_palloc_small(pool, size, 0);
    }

    return ch_palloc_large(pool, size);
}

void *
ch_pmemalign(ch_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    ch_pool_large_t  *large;

    p = memalign(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = ch_palloc_small(pool, sizeof(ch_pool_large_t), 1);
    if (large == NULL) {
        free(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


int
ch_pfree(ch_pool_t *pool, void *p)
{
    ch_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            free(l->alloc);
            l->alloc = NULL;

            return 0;
        }
    }

    return -1;
}


void *
ch_pcalloc(ch_pool_t *pool, size_t size)
{
    void *p;

    p = ch_palloc(pool, size);
    if (p) {
        memset(p, 0,size);
    }

    return p;
}


ch_pool_cleanup_t *
ch_pool_cleanup_add(ch_pool_t *p, size_t size)
{
    ch_pool_cleanup_t  *c;

    c = ch_palloc(p, sizeof(ch_pool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = ch_palloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    return c;
}

