/*
 * =====================================================================================
 *
 *       Filename:  ch_mpool_agent.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/12/2018 04:31:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_MPOOL_AGNENT_H
#define CH_MPOOL_AGNENT_H

typedef struct ch_mpool_agent_t ch_mpool_agent_t;

#include "ch_mpool.h"


struct ch_mpool_agent_t {

    struct list_head cache_list; 

    uint64_t max_cache_n;
    uint64_t cur_cache_n;

    uint64_t pool_size;
    uint64_t pre_alloc_n;
};

static inline int ch_mpool_agent_init(ch_mpool_agent_t *mpa,uint64_t max_cache_n,uint64_t pool_size,uint64_t pre_alloc_n){

    int i = 0;

    ch_pool_t *mp;

    INIT_LIST_HEAD(&mpa->cache_list);
    mpa->max_cache_n = max_cache_n==0?100000:max_cache_n;
    mpa->cur_cache_n = 0;
    mpa->pool_size = pool_size == 0?4096:pool_size;
    mpa->pre_alloc_n = pre_alloc_n==0?1000:pre_alloc_n;

    for(i = 0;i<mpa->pre_alloc_n;i++){


        mp = ch_pool_create(mpa->pool_size);

        if(mp){

            mpa->cur_cache_n = mpa->cur_cache_n+1;
            list_add(&mp->node,&mpa->cache_list);

        }
    }

    return 0;

}

static inline ch_pool_t *ch_mpool_agent_alloc(ch_mpool_agent_t *mpa){


    ch_pool_t *mp = NULL;

    if(!list_empty(&mpa->cache_list)){
        
        mp = list_first_entry(&mpa->cache_list,ch_pool_t,node);
        
        list_del(&mp->node);
        mpa->cur_cache_n = mpa->cur_cache_n-1;
    }else{

        mp = ch_pool_create(mpa->pool_size);
    }

    return mp;
}

static inline void ch_mpool_agent_free(ch_mpool_agent_t *mpa,ch_pool_t *mp){

    if(mpa->cur_cache_n<mpa->max_cache_n){

        ch_pool_reset(mp);
        list_add(&mp->node,&mpa->cache_list);
        mpa->cur_cache_n = mpa->cur_cache_n+1;

    }else{

        ch_pool_destroy(mp);
    }

}
#endif /* CH_MPOOL_AGNENT_H */
