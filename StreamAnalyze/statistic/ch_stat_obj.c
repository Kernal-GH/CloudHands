/*
 *
 *      Filename: ch_stat_obj.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-05 16:58:22
 * Last Modified: 2018-06-08 15:49:21
 */

#include "ch_stat_obj.h"

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

void ch_stat_obj_reset(ch_stat_obj_t *stat_obj){

	uint32_t i;
	ch_stat_entry_t *entry;
	ch_stat_obj_hdr_t *obj_hdr = stat_obj->obj_hdr;

	for(i = 0;i<obj_hdr->entry_num;i++){
	
		entry = stat_obj->stat_entris+i;
		ch_stat_entry_init(entry);
	}

}

void ch_stat_obj_handle(ch_stat_obj_t *stat_obj,uint32_t index,uint64_t pkt_size){


	ch_stat_entry_t *entry = ch_stat_obj_entry_find(stat_obj,index);

	if(entry == NULL)
		return;

	ch_stat_entry_value_update(entry,pkt_size);
}

void ch_stat_obj_dump(ch_stat_obj_t *stat_obj,uint32_t n,FILE *fp){

	ch_stat_entry_t *entry;
	uint32_t i;
	if(n>stat_obj->obj_hdr->entry_num)
		n = stat_obj->obj_hdr->entry_num;


	fprintf(fp,"%s:[",ch_stat_obj_type_name(stat_obj));
	for(i = 0;i<n;i++){

		entry = stat_obj->stat_entris+i;
		fprintf(fp,"(%lu,%lu)",(unsigned long)entry->st_n,(unsigned long)entry->st_v);
		if(i!=n-1)
			fprintf(fp,",");

	}

	fprintf(fp,"]\n\n");
}

ssize_t ch_stat_obj_out(ch_stat_obj_t *stat_obj,ch_data_output_t *dout,int type,uint32_t n){

	ssize_t rc,len = 0;

	ch_stat_entry_t *entry;
	uint32_t i;

	if(n>stat_obj->obj_hdr->entry_num)
		n = stat_obj->obj_hdr->entry_num;
	
	CH_DOUT_UINT16_WRITE(dout,type,len,rc);
	CH_DOUT_UINT32_WRITE(dout,n,len,rc);

	for(i = 0;i<n;i++){

		entry = stat_obj->stat_entris+i;

		CH_DOUT_UINT64_WRITE(dout,entry->st_n,len,rc);
		CH_DOUT_UINT64_WRITE(dout,entry->st_v,len,rc);
	}

	return len;
}
