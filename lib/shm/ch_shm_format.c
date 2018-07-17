/*
 * =====================================================================================
 *
 *       Filename:  ch_shm_format.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2017 04:31:36 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_log.h"
#include "ch_shm_format.h"

ch_shm_format_t * ch_shm_format_create(ch_pool_t *mp,ch_shm_interface_t *shm_int,
  uint32_t entry_size,
  uint32_t record_header_size,
  ch_shm_record_t *shm_record,
  void (*shm_record_write)(ch_shm_format_t *fmt,ch_shm_record_t *record),
  int (*shm_record_read)(ch_shm_entry_iterator_t *iter,ch_bin_format_t *bfmt,ch_shm_record_t *shm_record)){


   ch_shm_format_t *fmt = NULL;


   fmt = (ch_shm_format_t*)ch_pcalloc(mp,sizeof(ch_shm_format_t));
   fmt->mp = mp;
   fmt->shm_int = shm_int;
   fmt->has_header = 0;
   fmt->entry_size = entry_size;
   fmt->record_header_size = record_header_size;
   fmt->shm_record_write = shm_record_write;
   fmt->shm_record_read = shm_record_read;

   fmt->shm_record = shm_record;

   return fmt;
}

static inline int _is_record_size_over(ch_shm_format_t *fmt,ch_shm_record_t *record){

    uint32_t rem_entry_size = fmt->entry_size-CH_SHM_ENTRY_HEADER_LEN;
    uint32_t record_size = (uint32_t)record->record_size;

    return record_size > rem_entry_size;
}

static inline int _is_need_create_entry_header(ch_shm_format_t *fmt,ch_shm_record_t *record){

    ch_bin_format_t *bfmt = &fmt->cur_header.bfmt;
    
    if(fmt->has_header == 0)
        return 1;

    if(ch_bf_full(bfmt,(size_t)record->record_size))
        return 1;

    return 0;
}

static inline void _shm_entry_header_commit(ch_shm_format_t *fmt,ch_shm_entry_header_t *header){

    ch_bin_format_t *bfmt = &header->bfmt;
    ch_shm_entry_t *shm_entry = &header->shm_entry;
    ch_bf_reset(bfmt);

    ch_bf_uint32_write(bfmt,header->magic);
    ch_bf_uint32_write(bfmt,header->major_version);
    ch_bf_uint32_write(bfmt,header->minor_version);
    ch_bf_uint32_write(bfmt,header->entries_number);
    ch_bf_uint32_write(bfmt,header->entries_size);

    ch_shm_entry_commit(fmt->shm_int,shm_entry);
}

static inline void _shm_entry_header_init(ch_shm_entry_header_t *header){

    ch_bin_format_t *bfmt = &header->bfmt;
    ch_bf_init(bfmt,header->shm_entry.start,header->shm_entry.end);

    header->magic = FIMAGIC;
    header->major_version = MAJOR_VERSION;
    header->minor_version = MINJOR_VERSION;
    header->entries_size = 0;
    header->entries_number = 0;

    ch_bf_skip(bfmt,CH_SHM_ENTRY_HEADER_LEN);
}

static int _shm_entry_header_create(ch_shm_format_t *fmt){

    if(fmt->has_header){
        _shm_entry_header_commit(fmt,&fmt->cur_header);
        fmt->has_header = 0;
    }

    if(-1 == ch_shm_entry_get(fmt->shm_int,&(fmt->cur_header.shm_entry))){
        return -1;
    }

    fmt->has_header = 1;
    _shm_entry_header_init(&fmt->cur_header);
    /*ok*/
    return 0;
}

static void _shm_record_write(ch_shm_format_t *fmt,ch_shm_record_t *record){

    unsigned char *wdata;

    ch_shm_entry_header_t *header = &fmt->cur_header;
    ch_bin_format_t *bfmt = &header->bfmt;

    uint32_t bsize = (uint32_t)ch_bf_content_size(bfmt);

    ch_bf_uint32_write(bfmt,EIMAGIC);
    ch_bf_uint32_write(bfmt,ch_shm_record_len(fmt,record));

    fmt->shm_record_write(fmt,record);
    
    ch_bf_uint32_write(bfmt,record->data_offset);

    wdata = record->data == NULL ||record->data_len == 0?"":record->data;

    ch_bf_bytes_write(bfmt,wdata,record->data_len);

    uint32_t asize = (uint32_t)ch_bf_content_size(bfmt);

    header->entries_number+=1;
    header->entries_size += (asize-bsize);
}

int ch_shm_format_put(ch_shm_format_t *fmt,ch_shm_record_t *record){

   if(_is_record_size_over(fmt,record)){
   
        ch_log(CH_LOG_ERR,"The record size:%lu is too large!",record->record_size);
        return -2;
   }

    if(_is_need_create_entry_header(fmt,record)){

        if(-1 == _shm_entry_header_create(fmt)){

            ch_log(CH_LOG_ERR,"Cannot create a shm entry to store record!");
            return -1;
        }
    }

    _shm_record_write(fmt,record);

    /* ok */
    return 0;
}

static inline int _can_been_flush(ch_shm_format_t *fmt){

    ch_bin_format_t *bfmt = &fmt->cur_header.bfmt;
    if(fmt->has_header == 0)
       return 0;
    
    if(ch_bf_empty(bfmt))
       return 0;
    
    return 1;
}

void ch_shm_format_flush(ch_shm_format_t *fmt){
   
   if(_can_been_flush(fmt)){
      
      if(-1 == _shm_entry_header_create(fmt)){
         fmt->has_header = 0;
      
      }
   }
}

static inline int _shm_entry_header_read(ch_shm_entry_header_t *header,ch_bin_format_t *bfmt){

    header->magic = ch_bf_uint32_read(bfmt);
    if(header->magic!=FIMAGIC){
        ch_log(CH_LOG_ERR,"Shm entry header Magic:%lu is invalid!",header->magic);
        return -1;
    }

    header->major_version = ch_bf_uint32_read(bfmt);
    header->minor_version = ch_bf_uint32_read(bfmt);
    header->entries_number = ch_bf_uint32_read(bfmt);
    header->entries_size = ch_bf_uint32_read(bfmt);

    return 0;
}

static inline int has_next(ch_shm_entry_iterator_t *iter){

    if(iter->header == NULL)
        return 0;

    if(iter->entries_count>=iter->header->entries_number)
        return 0;

    return 1;
}

static inline int  _record_read(ch_shm_entry_iterator_t *iter,ch_bin_format_t *bfmt,ch_shm_record_t *record){

    uint32_t magic;
    uint32_t record_size;

    magic = ch_bf_uint32_read(bfmt);
    
    if(magic!=EIMAGIC){
        ch_log(CH_LOG_ERR,"record magic:%lu is invalid!",magic);
        iter->header = NULL;
        return -1;
    }
    
    record_size = ch_bf_uint32_read(bfmt);
    if(iter->fmt->shm_record_read(iter,bfmt,record)){
	
		/*error*/
		return -1;
	}
    
    record->magic = magic;
    record->record_size = record_size;
    record->data_offset = ch_bf_uint32_read(bfmt);

    ch_bf_bytes_read(bfmt,(unsigned char**)(&record->data),&record->data_len);

    return 0;
}

static ch_shm_record_t * record_next(ch_shm_entry_iterator_t *iter){

	int rc;

    ch_shm_record_t *record = iter->fmt->shm_record;
    ch_shm_entry_header_t *header = iter->header;

    ch_bin_format_t *bfmt = &header->bfmt;

    if(has_next(iter) == 0){
        return NULL;
    }

    rc =  _record_read(iter,bfmt,record);

    if(rc == 0){

        iter->entries_count+=1;
		return record;
    }

    return NULL;
}

ch_shm_entry_iterator_t * ch_shm_entry_iterator_prefare(ch_shm_format_t *fmt){

    ch_shm_entry_t *shm_entry = &fmt->cur_header.shm_entry;
	ch_shm_entry_iterator_t *g_iter_ptr = &fmt->shm_iter;

    g_iter_ptr->fmt = fmt;
    g_iter_ptr->header = NULL;
    g_iter_ptr->next = record_next;
    g_iter_ptr->entries_count = 0;

    if( -1 == ch_shm_entry_get(fmt->shm_int,shm_entry)){
        return NULL;
    }

    ch_bf_init(&fmt->cur_header.bfmt,shm_entry->start,shm_entry->end);

    if(-1 == _shm_entry_header_read(&fmt->cur_header,&fmt->cur_header.bfmt))
        return NULL;

    g_iter_ptr->header = &fmt->cur_header;

    return g_iter_ptr;
}

void ch_shm_entry_iterator_commit(ch_shm_format_t *fmt,ch_shm_entry_iterator_t *iter){

    if(iter->header){
        ch_shm_entry_t *shm_entry = &iter->header->shm_entry;
        ch_shm_entry_commit(fmt->shm_int,shm_entry);
        iter->header = NULL;
        iter->entries_count = 0;
    }

}
