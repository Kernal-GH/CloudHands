/*
 *
 *      Filename: ch_wb_list.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-03 16:51:55
 * Last Modified: 2018-09-03 16:51:55
 */

#ifndef CH_WB_LIST_H
#define CH_WB_LIST_H
 
typedef struct ch_wb_list_t ch_wb_list_t;
typedef struct ch_wb_list_iter_t ch_wb_list_iter_t;

#include "ch_wb_memory.h"


struct ch_wb_list_iter_t {

	ch_wb_list_t *wb_list;

	void *(*next)(ch_wb_list_iter_t *iter);

};

struct ch_wb_list_t {

	ch_wb_memory_t wbm;

	int is_on;
	int is_black_list;

	int (*load)(ch_wb_list_t *wb_list);
	void (*unload)(ch_wb_list_t *wb_list);

	int (*add_range_num)(ch_wb_list_t *wb_list,uint64_t start,uint64_t end);
	int (*add_str)(ch_wb_list_t *wb_list,const char *v);
	
	void (*del)(ch_wb_list_t *wb_list,uint64_t id);

	int (*is_match_str)(ch_wb_list_t *wb_list,const char *key);
	int (*is_match_num)(ch_wb_list_t *wb_list,uint64_t v);

	ch_wb_list_iter_t *(*wb_list_iter_prepare)(ch_wb_list_t *wb_list);

};

extern int ch_wb_list_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize,
	int is_on,
	int is_black_list,
	int (*load)(ch_wb_list_t *wb_list),
	void (*unload)(ch_wb_list_t *wb_list),
	int (*add_range_num)(ch_wb_list_t *wb_list,uint64_t start,uint64_t end),
	int (*add_str)(ch_wb_list_t *wb_list,const char *v),
	void (*del)(ch_wb_list_t *wb_list,uint64_t id),
	int (*is_match_str)(ch_wb_list_t *wb_list,const char *key),
	int (*is_match_num)(ch_wb_list_t *wb_list,uint64_t v),
	ch_wb_list_iter_t *(*wb_list_iter_prepare)(ch_wb_list_t *wb_list));

extern void ch_wb_list_fin(ch_wb_list_t *wb_list);

extern int ch_wb_list_add_range_num(ch_wb_list_t *wb_list,uint64_t start,uint64_t end);

extern int ch_wb_list_add_str(ch_wb_list_t *wb_list,const char *v);

extern void ch_wb_list_del(ch_wb_list_t *wb_list,uint64_t id);

extern int ch_wb_list_is_match_str(ch_wb_list_t *wb_list,const char *key);

extern int ch_wb_list_is_match_num(ch_wb_list_t *wb_list,uint64_t v);

extern ch_wb_list_iter_t * ch_wb_list_iter_prepare(ch_wb_list_t *wb_list);

#endif /*CH_WB_LIST_H*/
