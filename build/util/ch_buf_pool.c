/*
 *
 *      Filename: ch_buf_pool.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-20 20:47:03
 * Last Modified: 2016-07-21 03:06:22
 */

#include "ch_buf_pool.h"
#include "ch_util.h"

static inline struct ch_list_head* _free_entry_header(ch_buf_pool_t *bp,size_t i){

	return bp->frees+i;
}

ch_buf_pool_t * ch_buf_pool_create(apr_pool_t *mp,
	void (*expand_init)(ch_buf_t *nb,ch_buf_t *ob,void *priv_data)){

	struct ch_list_head *h;
	size_t i;
	ch_buf_pool_t *bp = NULL;
	size_t free_size = 0;
	size_t b_max_power = 64;


	free_size = sizeof(*bp)+(b_max_power+1)*sizeof(bp->frees[0]);

	bp = (ch_buf_pool_t*)apr_palloc(mp,free_size);

	bp->mp = mp;
	bp->b_max_power = b_max_power;
	bp->allocated_size = 0;
	bp->expand_init = expand_init;

	for(i = 0;i<=b_max_power;i++){

		h = _free_entry_header(bp,i);
		CH_INIT_LIST_HEAD(h);
	}

	return bp;
}

static inline ch_buf_t * _get_buf_from_list(struct ch_list_head *h,size_t bsize){

	ch_buf_t *b = NULL,*tb;
	
	list_for_each_entry_safe(b,tb,h,node){
	
		if(ch_buf_size(b)>=bsize){
		
			list_del(&b->node);
			return b;
		}
	}

	return NULL;
}

static ch_buf_t * _get_buf_from_free_list(ch_buf_pool_t *bp,size_t bsize){

	ch_buf_t *b = NULL;
	struct ch_list_head *h;
	size_t i,pbsize;

	pbsize = ch_align64pow2(bsize);
	i = ch_uint64_zshu(pbsize);

	for(;i<=bp->b_max_power;i++){
	
		h = _free_entry_header(bp,i);
		b = _get_buf_from_list(h,bsize);
		if(b){
		
			ch_buf_reset(b);
			return b;
		}
	}

	return NULL;
}

static inline ch_buf_t * _new_buf(ch_buf_pool_t *bp,size_t bsize){

	ch_buf_t *b;

	size_t r_size = bsize+sizeof(ch_buf_t);
	b = (ch_buf_t*)apr_palloc(bp->mp,r_size);
	
	ch_buf_init(b,bsize);

	bp->allocated_size += r_size;

	return b;
}

ch_buf_t *ch_buf_pool_alloc(ch_buf_pool_t *bp,size_t bsize){

	ch_buf_t *b = NULL;

	b = _get_buf_from_free_list(bp,bsize);
	
	if(b){

		return b;
	}

	return _new_buf(bp,bsize);
}

void ch_buf_pool_free(ch_buf_pool_t *bp,ch_buf_t *b){

	struct ch_list_head *h;
	size_t i;
	size_t bsize = ch_buf_size(b);
	 
	i = ch_uint64_zshu(ch_align64pow2(bsize));
	if(i<bp->b_max_power){
	
		h = _free_entry_header(bp,i);
		list_add(&b->node,h);
	}
}

ch_buf_t *ch_buf_expand(ch_buf_pool_t *bp,ch_buf_t *ob,size_t incr_size,void *priv_data){

	size_t bsize = ch_buf_size(ob)+incr_size;

	ch_buf_t *nb = NULL;

	nb = ch_buf_pool_alloc(bp,bsize);
	if(nb == NULL)
		return NULL;

	bp->expand_init(nb,ob,priv_data);

	ch_buf_pool_free(bp,ob);

	return nb;
}

