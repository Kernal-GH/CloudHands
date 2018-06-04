/*
 * =====================================================================================
 *
 *       Filename:  ch_buf_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月12日 11时56分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_BUF_POOL_H
#define CH_BUF_POOL_H

typedef struct ch_buf_pool_t ch_buf_pool_t;

#include <apr_pools.h>
#include "ch_buf.h"
#include "ch_list.h"

struct ch_buf_pool_t {

    apr_pool_t *mp;
    size_t b_max_power;
    size_t allocated_size;

    void (*expand_init)(ch_buf_t *new_buf,ch_buf_t *old_buf,void *priv_data);
    
    struct ch_list_head frees[0];
};

extern ch_buf_pool_t * ch_buf_pool_create(apr_pool_t *mp,
	void (*expand_init)(ch_buf_t *nb,ch_buf_t *ob,void *priv_data));

extern ch_buf_t *ch_buf_pool_alloc(ch_buf_pool_t *bp,size_t bsize);

extern void ch_buf_pool_free(ch_buf_pool_t *bp,ch_buf_t *b);

extern ch_buf_t *ch_buf_expand(ch_buf_pool_t *bp,ch_buf_t *ob,size_t incr_size,void *priv_data);

#endif /*CH_BUF_POOL_H*/
