/*
 *
 *      Filename: ch_wb_list_str.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-05 15:19:58
 * Last Modified: 2018-09-05 17:02:06
 */

#include "ch_wb_list_str.h"
#include <string.h>

static int list_str_add(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *value){

	
	ch_wb_list_str_entry_t *str_entry = (ch_wb_list_str_entry_t*)entry;
	const char *av = (const char*)value;
	size_t len;
	if(av == NULL||(len=strlen(av))==0)
		return -1;

	if(len+1+sizeof(ch_wb_list_str_entry_t)>wb_list->header->entry_size)
		return -1;

	memcpy(str_entry->value,av,len);
	str_entry->value[len]=0;

	return 0;
}

static void list_str_entry_copy(ch_wb_list_t *wb_list,ch_wb_list_entry_t *d_entry,ch_wb_list_entry_t *s_entry){


	/*unused*/
	wb_list = wb_list;

	ch_wb_list_str_entry_t *d_str_entry = (ch_wb_list_str_entry_t*)d_entry;
	ch_wb_list_str_entry_t *s_str_entry = (ch_wb_list_str_entry_t*)s_entry;

	memcpy(d_str_entry->value,s_str_entry->value,strlen(s_str_entry->value)+1);

}

static int list_str_is_match(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,void *match_value){

	/*unused*/
	wb_list = wb_list;

	ch_wb_list_str_entry_t *str_entry =  (ch_wb_list_str_entry_t*)entry;
	const char *mv = (const char *)match_value;

	if(mv == NULL || strlen(mv)==0)
		return 0;

	return strncmp(str_entry->value,mv,strlen(mv))==0;
}

static void list_str_dump(ch_wb_list_t *wb_list,ch_wb_list_entry_t *entry,FILE *out){

	/*unused*/
	wb_list = wb_list;

	ch_wb_list_str_entry_t *str_entry = (ch_wb_list_str_entry_t*)entry;

	fprintf(out,"Dump The String Entry Info:\n");
	
	fprintf(out,"StringEntry.Value:%s\n",str_entry->value);

}

int ch_wb_list_str_init(ch_wb_list_t *wb_list,const char *mmap_fname,size_t msize,size_t max_str_len){

	return ch_wb_list_init(wb_list,mmap_fname,msize,sizeof(ch_wb_list_str_entry_t)+max_str_len+1,
		list_str_add,
		list_str_entry_copy,
		list_str_is_match,
		list_str_dump);

}

