/*
 *
 *      Filename: ch_wb_list.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-03 18:16:04
 * Last Modified: 2018-09-03 19:40:46
 */

#include "ch_wb_list.h"

int ch_wb_list_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize,
	int is_on,
	int is_black_list,
	int (*load)(ch_wb_list_t *wb_list),
	void (*unload)(ch_wb_list_t *wb_list),
	int (*add_range_num)(ch_wb_list_t *wb_list,uint64_t start,uint64_t end),
	int (*add_str)(ch_wb_list_t *wb_list,const char *v),
	void (*del)(ch_wb_list_t *wb_list,uint64_t id),
	int (*is_match_str)(ch_wb_list_t *wb_list,const char *key),
	int (*is_match_num)(ch_wb_list_t *wb_list,uint64_t v),
	ch_wb_list_iter_t *(*wb_list_iter_prepare)(ch_wb_list_t *wb_list)) {


	if(ch_wb_memory_init(&wb_list->wbm,mmap_fname,msize))
		return -1;

	wb_list->is_on = is_on;
	wb_list->is_black_list = is_black_list;
	wb_list->load = load;
	wb_list->unload = unload;
	wb_list->add_range_num = add_range_num;
	wb_list->add_str = add_str;
	wb_list->del = del;
	wb_list->is_match_str = is_match_str;
	wb_list->is_match_num = is_match_num;
	wb_list->wb_list_iter_prepare = wb_list_iter_prepare;

	if(wb_list->load)
		return wb_list->load(wb_list);

	return 0;
}


void ch_wb_list_fin(ch_wb_list_t *wb_list) {

	if(wb_list->unload)
		wb_list->unload(wb_list);

	ch_wb_memory_fin(&wb_list->wbm);


}

int ch_wb_list_add_range_num(ch_wb_list_t *wb_list,uint64_t start,uint64_t end){

	if(wb_list->add_range_num == NULL)
		return -1;

	return wb_list->add_range_num(wb_list,start,end);

}

int ch_wb_list_add_str(ch_wb_list_t *wb_list,const char *v){

	if(wb_list->add_str == NULL)
		return -1;
	return wb_list->add_str(wb_list,v);
}

void ch_wb_list_del(ch_wb_list_t *wb_list,uint64_t id){

	if(wb_list->del)
		wb_list->del(wb_list,id);
}

int ch_wb_list_is_match_str(ch_wb_list_t *wb_list,const char *key){

	if(wb_list->is_match_str)
		return wb_list->is_match_str(wb_list,key);

	return 0;
}

int ch_wb_list_is_match_num(ch_wb_list_t *wb_list,uint64_t v){

	if(wb_list->is_match_num)
		return wb_list->is_match_num(wb_list,v);
	return 0;
}

ch_wb_list_iter_t * ch_wb_list_iter_prepare(ch_wb_list_t *wb_list){

	if(wb_list->wb_list_iter_prepare)
		return wb_list->wb_list_iter_prepare(wb_list);

	return NULL;
}

