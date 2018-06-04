/*
 * =====================================================================================
 *
 *       Filename:  ch_buf.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月12日 10时29分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_BUF_H
#define CH_BUF_H

typedef struct ch_buf_t ch_buf_t;

#include "ch_list.h"

struct ch_buf_t {

	struct ch_list_head node;

    void *start;
    void *pos;
    void *last;
    void *end;
};


static inline size_t ch_buf_size(ch_buf_t *b){

	return (size_t)(b->end-b->start);
}

static inline void ch_buf_init(ch_buf_t *bf,size_t bsize){

	bf->start = (void*)(bf+1);
	bf->pos = bf->start;
	bf->last = bf->start;
	bf->end = bf->start+bsize;
}

static inline void ch_buf_reset(ch_buf_t *bf){

	bf->pos = bf->start;
	bf->last = bf->start;
}

#endif /*CH_BUF_H*/
