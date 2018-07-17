/*
 *
 *      Filename: ch_packet_record.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-10 14:28:42
 * Last Modified: 2018-07-17 13:43:46
 */

#include "ch_packet_record.h"
#include "ch_log.h"
#include "ch_shm_mmap.h"
#include "ch_shm_memory.h"


static void _pkt_record_write(ch_shm_format_t *fmt,ch_shm_record_t *rcd){

    ch_shm_entry_header_t *header = &fmt->cur_header;
    ch_bin_format_t *bfmt = &header->bfmt;

	ch_packet_record_t *pkt_rcd = (ch_packet_record_t*)rcd;

    ch_bf_uint8_write(bfmt,pkt_rcd->type);
    ch_bf_uint16_write(bfmt,pkt_rcd->meta_data_size);

    ch_bf_uint64_write(bfmt,pkt_rcd->time);

}

static int  _pkt_record_read(ch_shm_entry_iterator_t *iter,ch_bin_format_t *bfmt,ch_shm_record_t *shm_record){

	ch_packet_record_t *pkt_rcd = (ch_packet_record_t*)shm_record;

    pkt_rcd->type = ch_bf_uint8_read(bfmt); 
	pkt_rcd->meta_data_size = ch_bf_uint16_read(bfmt);
    pkt_rcd->time = ch_bf_uint64_read(bfmt);
    

	return 0;
}


ch_shm_format_t * ch_shm_format_pkt_with_mmap_create(ch_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,
	uint64_t priv_data_size,int is_write){

	ch_shm_interface_t *shm_int = NULL;
	ch_packet_record_t *pkt_rcd = (ch_packet_record_t*)ch_pcalloc(mp,sizeof(*pkt_rcd));

	shm_int = ch_shm_mmap_create(mp,fname,fsize,entry_size,priv_data_size,is_write);
	if(shm_int == NULL){
	
		ch_log(CH_LOG_ERR,"Create a mmap file failed for packet!");
		return NULL;
	}

	return ch_shm_format_create(mp,shm_int,entry_size,
		CH_PKT_RECORD_HEADER_SIZE,
		(ch_shm_record_t*)pkt_rcd,
		_pkt_record_write,
		_pkt_record_read);

}

ch_shm_format_t * ch_shm_format_pkt_with_shm_create(ch_pool_t *mp,const char *key,int proj_id,uint64_t size,
	uint64_t entry_size,uint64_t priv_data_size,int is_write){

	ch_packet_record_t *pkt_rcd = (ch_packet_record_t*)ch_pcalloc(mp,sizeof(*pkt_rcd));
	ch_shm_interface_t *shm_int = NULL;
	
	shm_int = ch_shm_memory_create(mp,key,proj_id,size,entry_size,priv_data_size,is_write);
	if(shm_int == NULL){
	
		ch_log(CH_LOG_ERR,"Create a shm memory failed for packet!");
		return NULL;
	}

	return ch_shm_format_create(mp,shm_int,entry_size,
		CH_PKT_RECORD_HEADER_SIZE,
		(ch_shm_record_t*)pkt_rcd,
		_pkt_record_write,
		_pkt_record_read);

}

int ch_packet_record_put(ch_shm_format_t *shm_fmt,ch_packet_record_t *pkt_rcd,void *data,size_t dlen){

	int rc;

	ch_shm_record_t *shm_rcd = &pkt_rcd->shm_rcd;

	shm_rcd->magic = EIMAGIC;

	shm_rcd->record_size = CH_PKT_RECORD_HEADER_SIZE+dlen;

	shm_rcd->data_offset = 0;

	shm_rcd->data_len = dlen;
	shm_rcd->data = data;

    for(;;){

        rc = ch_shm_format_put(shm_fmt,(ch_shm_record_t*)pkt_rcd);
        if(rc == 0){
            return 0;
        }

        if(rc == -2){

            ch_log(CH_LOG_ERR,"The data is too large:%lu,discard it!",dlen);

            return -2;
        }

        ch_log(CH_LOG_WARN,"The shm is full,sleep and try again!");

        usleep(5);
    }

	return 0;

}
