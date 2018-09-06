/*
 *
 *      Filename: ch_wb_list.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-03 18:16:04
 * Last Modified: 2018-09-06 18:07:17
 */

#include <fcntl.h>
#include <unistd.h>
#include "ch_wb_list.h"

int ch_wb_list_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize,uint32_t entry_size,
	int (*add)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *value),
	void (*entry_copy)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *d_entry,ch_wb_list_entry_t *s_entry),
	int (*is_match)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *match_value),
	void (*dump)(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,FILE *out)){

	int existed = access(mmap_fname,F_OK) == 0;

	if(ch_wb_memory_init(&wb_list->wbm,mmap_fname,msize))
		return -1;

	wb_list->add = add;
	wb_list->entry_copy = entry_copy;
	wb_list->is_match = is_match;
	wb_list->dump = dump;
	
	wb_list->header = (ch_wb_list_header_t*)ch_wb_memory_alloc(&wb_list->wbm,sizeof(ch_wb_list_header_t));
	if(wb_list->header == NULL)
		return -1;


	wb_list->header->entries_num = (wb_list->wbm.end-wb_list->wbm.pos)/entry_size;

	if(!existed){

		wb_list->header->is_on = 0;
		wb_list->header->free_entry_pos = 0;

		wb_list->header->entry_size = entry_size;
	
		wb_list->header->next_id = 12345;
	}

	wb_list->entries =(void*)(wb_list->header+1);


	return 0;
}


void ch_wb_list_fin(ch_wb_list_t *wb_list) {

	ch_wb_memory_fin(&wb_list->wbm);


}

uint64_t ch_wb_list_add(ch_wb_list_t *wb_list,void *value){

	int rc;

	ch_wb_list_entry_t *entry = ch_wb_list_entry_get(wb_list,wb_list->header->free_entry_pos);
	if(entry == NULL)
		return 0;

	entry->id = ch_wb_list_alloc_id(wb_list);

	rc = wb_list->add(wb_list,entry,value);

	if(rc == 0){
	
		/*ok*/
		wb_list->header->free_entry_pos +=1;
	}


	return rc?0:entry->id;
}


static void entries_move(ch_wb_list_t *wb_list,ch_wb_list_entry_t *start_entry){

	ch_wb_list_entry_t *cur_entry = start_entry;
	ch_wb_list_entry_t *next_entry = (ch_wb_list_entry_t*)ch_wb_list_entry_next(wb_list,(void*)start_entry);

	while(next_entry){

		cur_entry->id = next_entry->id;
		cur_entry = next_entry;
		next_entry = (ch_wb_list_entry_t*)ch_wb_list_entry_next(wb_list,(void*)cur_entry);   
	}


}

static ch_wb_list_entry_t *_find_entry(ch_wb_list_t *wb_list,uint64_t id){

	ch_wb_list_entry_t *entry;

	ch_wb_list_for_each_entry(wb_list,entry,ch_wb_list_entry_t){
	
		if(entry->id == id)
			return entry;
	}

	return NULL;
}

void ch_wb_list_del(ch_wb_list_t *wb_list,uint64_t id){

	ch_wb_list_entry_t *entry;

	if(ch_wb_list_empty(wb_list))
		return;

	entry = _find_entry(wb_list,id);
	if(entry == NULL)
		return;

	entries_move(wb_list,entry);
	wb_list->header->free_entry_pos-=1;

}

int ch_wb_list_is_match(ch_wb_list_t *wb_list,void *match_value){

	ch_wb_list_entry_t *entry;

	if(ch_wb_list_empty(wb_list)||wb_list->header->is_on==0)
		return 0;

	ch_wb_list_for_each_entry(wb_list,entry,ch_wb_list_entry_t){
	
		if(wb_list->is_match(wb_list,entry,match_value))
			return 1;
	}

	return 0;
}

void ch_wb_list_dump(ch_wb_list_t *wb_list,FILE *out){

	ch_wb_list_entry_t *entry;
	fprintf(out,"Dump The White&Black List Informations:\n");
	fprintf(out,"WBList is on:%s\n",wb_list->header->is_on?"on":"off");
	fprintf(out,"Entries Number:%lu\n",(unsigned long)wb_list->header->entries_num);
	fprintf(out,"Current Free Entry Pos:%lu\n",(unsigned long)wb_list->header->free_entry_pos);
	fprintf(out,"Entry Size:%lu\n",(unsigned long)wb_list->header->entry_size);

	fprintf(out,"Dump The Entries Informations:\n");
	ch_wb_list_for_each_entry(wb_list,entry,ch_wb_list_entry_t){

		fprintf(out,"entry.id:%lu\n",(unsigned long)entry->id);
		wb_list->dump(wb_list,entry,out);

	}


}
