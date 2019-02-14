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
 
typedef struct ch_wb_list_header_t ch_wb_list_header_t;
typedef struct ch_wb_list_t ch_wb_list_t;
typedef struct ch_wb_list_entry_t ch_wb_list_entry_t;

#include "ch_wb_memory.h"

#pragma pack(push,1)
struct ch_wb_list_header_t {

	int is_on;
	uint32_t entries_num;
	uint32_t free_entry_pos;

	uint32_t entry_size;
	uint64_t next_id;

};
#pragma pack(pop)

struct ch_wb_list_entry_t {

	uint64_t id;

};

struct ch_wb_list_t {

	ch_wb_memory_t wbm;
	ch_wb_list_header_t *header;
	
	void *entries;

	int (*add)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *value);
	
	void (*entry_copy)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *d_entry,ch_wb_list_entry_t *s_entry);

	int  (*is_match)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *match_value);
	void (*dump)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,FILE *out);

};

#define ch_wb_list_empty(wb_list) (wb_list->header->free_entry_pos == 0)

static inline ch_wb_list_t * ch_wb_list_should_match_list(ch_wb_list_t *w_list,ch_wb_list_t *b_list,int *is_white){

	/*only enable a list*/
	if(w_list->header->is_on==b_list->header->is_on)
		return NULL;

	if(w_list->header->is_on){
		if(ch_wb_list_empty(w_list))
			return NULL;

		*is_white = 1;

		return w_list;

	}else if(b_list->header->is_on){
	
		if(ch_wb_list_empty(b_list))
			return NULL;

		*is_white = 0;
		return b_list;

	}else{
	
		/**/
		return NULL;
	}

}

static inline uint64_t ch_wb_list_alloc_id(ch_wb_list_t *wb_list){

	uint64_t id = wb_list->header->next_id;

	wb_list->header->next_id = (wb_list->header->next_id+1)%UINT64_MAX;

	 return id;
}

static inline void *ch_wb_list_entry_get(ch_wb_list_t *wb_list,uint32_t indx){

	void *entry = wb_list->entries+indx*wb_list->header->entry_size;
	if(entry>=wb_list->wbm.end)
		return NULL;

	return entry;
}

static inline void *ch_wb_list_entry_first(ch_wb_list_t *wb_list){

	if(wb_list->header->free_entry_pos == 0)
		return NULL;

	return wb_list->entries;
}

static inline void *ch_wb_list_entry_next(ch_wb_list_t *wb_list,void *entry){

	void *free_entry = ch_wb_list_entry_get(wb_list,wb_list->header->free_entry_pos);
	void *next_entry = entry+wb_list->header->entry_size;

	return next_entry>=free_entry?NULL:next_entry;
}


#define ch_wb_list_for_each_entry(wb_list,entry,type) \
	for(entry = (type*)ch_wb_list_entry_first(wb_list); \
		entry!=NULL; \
		entry = (type*)ch_wb_list_entry_next(wb_list,(void*)entry))

#define ch_wb_list_is_on_set(wb_list,v) ((wb_list)->header->is_on = v) 

extern int ch_wb_list_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize,uint32_t entry_size,
	int (*add)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *value),
	void (*entry_copy)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *d_entry,ch_wb_list_entry_t *s_entry),
	int (*is_match)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *match_value),
	void (*dump)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,FILE *out));

extern void ch_wb_list_fin(ch_wb_list_t *wb_list);

extern uint64_t ch_wb_list_add(ch_wb_list_t *wb_list,void *value);

extern void ch_wb_list_del(ch_wb_list_t *wb_list,uint64_t id);

extern int ch_wb_list_is_match(ch_wb_list_t *wb_list,void *match_value);

extern void ch_wb_list_dump(ch_wb_list_t *wb_list,FILE *out);

static inline int ch_wb_list_is_accept(ch_wb_list_t *w_list,ch_wb_list_t *b_list,
	void *s_match,void*d_match){

	int is_white;

	ch_wb_list_t *m_list = ch_wb_list_should_match_list(w_list,b_list,&is_white);

	if(m_list == NULL)
		return 1;

	int is_match = ch_wb_list_is_match(m_list,s_match)||ch_wb_list_is_match(m_list,d_match);

	return is_white?is_match:!is_match;
}

#endif /*CH_WB_LIST_H*/
