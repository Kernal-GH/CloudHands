/*
 * =====================================================================================
 *
 *       Filename:  ch_shm_interface.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2017 06:20:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_log.h"
#include "ch_shm_interface.h"
#include "ch_constants.h"
#include "ch_util.h"

static inline uint64_t _shm_entries_count_get(ch_shm_interface_t *shm_int,uint64_t hsize,uint64_t entry_size){

	return (shm_int->size-hsize)/entry_size;
} 

static void  _shm_header_init(ch_shm_interface_t *shm_int,uint64_t entry_size,uint64_t priv_data_size){

	ch_shm_header_t *mh = shm_int->shm_header;
	uint64_t hsize = (uint64_t)ch_align_up(sizeof(ch_shm_header_t)+priv_data_size,shm_int->page_size);

	mh->shm_entries_start = hsize;
	mh->shm_entries_count = _shm_entries_count_get(shm_int,hsize,entry_size);
	mh->shm_entries_count_cur = 0;
    mh->shm_entry_size = entry_size;
	mh->shm_write_entry_pos = 0;
	mh->shm_read_entry_pos = 0;
	mh->shm_priv_data_size = priv_data_size;
    mh->shm_last_write_time = 0;
    mh->shm_last_read_time = 0;
    mh->shm_last_write_ok_time = 0;
    mh->shm_last_read_ok_time = 0;
}

void ch_shm_init(ch_shm_interface_t *shm_int,void *address,
  uint64_t size,uint64_t entry_size,uint64_t priv_data_size,uint64_t page_size,int is_write){

   shm_int->page_size = page_size;
   shm_int->size = size;
   shm_int->shm_header = (ch_shm_header_t*)address;
   shm_int->is_write = is_write?1:0;

   if(is_write){
   
      _shm_header_init(shm_int,entry_size,priv_data_size);
   }

}

static inline int _shm_entry_can_get(ch_shm_interface_t *shm_int){

    /*write,full*/
    if((shm_int->is_write)&&(ch_shm_full(shm_int))){
        return 0;
    }

    /*read,empty*/
    if((shm_int->is_write == 0)&&(ch_shm_empty(shm_int))){
        return 0;
    }

    return 1;
}

static inline uint64_t _shm_entry_index_get(ch_shm_interface_t *shm_int){

    if(shm_int->is_write){

        return shm_int->shm_header->shm_write_entry_pos;
    }else{
        return shm_int->shm_header->shm_read_entry_pos;
    }
}

static int  _shm_entry_init(ch_shm_interface_t *shm_int,ch_shm_entry_t *entry,uint64_t indx){

    uint64_t offset = 0;
    void *addr = NULL;

    offset = ch_shm_entry_pos_offset(shm_int,indx);

    addr = shm_int->shm_address_get(shm_int,offset);

    if(addr == NULL){
        return -1;
    }

    entry->shm_int = shm_int;
    entry->entry_index = indx;
    entry->start = addr;
    entry->pos = entry->start;
    entry->end = entry->start+shm_int->shm_header->shm_entry_size;

    return 0;
}


int ch_shm_entry_get(ch_shm_interface_t *shm_int,ch_shm_entry_t *entry){

    uint64_t indx;

    uint64_t time = ch_get_current_timems();

    if(shm_int->is_write){

        shm_int->shm_header->shm_last_write_time = time;
    }else{
        shm_int->shm_header->shm_last_read_time = time;
    }

    if(_shm_entry_can_get(shm_int) == 0){

        ch_log(CH_LOG_INFO,"No buf get from shm used to %s",shm_int->is_write?"write":"read");
        return -1;
    }

    indx = _shm_entry_index_get(shm_int);

    if(shm_int->is_write){

        shm_int->shm_header->shm_last_write_ok_time = time;
    }else{
        shm_int->shm_header->shm_last_read_ok_time = time;
    }

    return _shm_entry_init(shm_int,entry,indx);
}

void ch_shm_entry_commit(ch_shm_interface_t *shm_int,ch_shm_entry_t *entry){

    int c = shm_int->is_write?1:-1;
    ch_shm_entry_pos_update(shm_int,1,c);

    shm_int->shm_address_free(shm_int,entry->start);

    entry->shm_int = NULL;
    entry->entry_index = 0;
    entry->start = NULL;
    entry->pos = NULL;
    entry->end = NULL;
}
