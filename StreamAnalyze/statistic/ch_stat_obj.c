/*
 *
 *      Filename: ch_stat_obj.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-05 16:58:22
 * Last Modified: 2018-06-05 18:06:50
 */

#include "ch_stat_entry.h"

void ch_stat_obj_init(ch_stat_obj_t *stat_obj,void *addr,uint32_t obj_type,uint32_t entry_num){

	uint32_t i;
	ch_stat_entry_t *entry;
	ch_stat_obj_hdr_t *obj_hdr = (ch_stat_obj_hdr_t*)addr;

	stat_obj->obj_hdr = obj_hdr;

	obj_hdr->obj_type = obj_type;
	obj_hdr->entry_num = entry_num;
	obj_hdr->entry_size = sizeof(ch_stat_entry_t);

	stat_obj->stat_entris = (ch_stat_entry_t*)(obj_hdr+1);

	for(i = 0;i<entry_num;i++){
	
		entry = stat_obj->stat_entris+i;
		ch_stat_entry_init(entry);
	}

}

void ch_stat_obj_load(ch_stat_obj_t *stat_obj,void *addr) {

	ch_stat_obj_hdr_t *obj_hdr = (ch_stat_obj_hdr_t*)addr;

	stat_obj->obj_hdr = obj_hdr;

	stat_obj->stat_entris = (ch_stat_entry_t*)(obj_hdr+1);


}

void ch_stat_obj_reset(ch_stat_obj_t *stat_obj,void *addr){

	uint32_t i;
	ch_stat_entry_t *entry;
	ch_stat_obj_hdr_t *obj_hdr = (ch_stat_obj_hdr_t*)addr;

	stat_obj->obj_hdr = obj_hdr;

	stat_obj->stat_entris = (ch_stat_entry_t*)(obj_hdr+1);

	for(i = 0;i<entry_num;i++){
	
		entry = stat_obj->stat_entris+i;
		ch_stat_entry_init(entry);
	}

}

