/*
 *
 *      Filename: ch_tcp_record.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-29 11:43:49
 * Last Modified: 2018-07-17 13:39:58
 */

#include "ch_tcp_record.h"
#include "ch_log.h"
#include "ch_shm_mmap.h"
#include "ch_shm_memory.h"

static void _tcp_record_write(ch_shm_format_t *fmt,ch_shm_record_t *rcd){

    ch_shm_entry_header_t *header = &fmt->cur_header;
    ch_bin_format_t *bfmt = &header->bfmt;

	ch_tcp_record_t *tcp_rcd = (ch_tcp_record_t*)rcd;

    ch_bf_uint8_write(bfmt,tcp_rcd->packet_type);
    ch_bf_uint8_write(bfmt,tcp_rcd->session_direct);
    ch_bf_uint32_write(bfmt,tcp_rcd->protocol_id);
    ch_bf_uint64_write(bfmt,tcp_rcd->session_id);
    ch_bf_uint64_write(bfmt,tcp_rcd->time);
    ch_bf_uint32_write(bfmt,tcp_rcd->src_ip);
    ch_bf_uint32_write(bfmt,tcp_rcd->dst_ip);
    ch_bf_uint16_write(bfmt,tcp_rcd->src_port);
    ch_bf_uint16_write(bfmt,tcp_rcd->dst_port);
	ch_bf_uint64_write(bfmt,tcp_rcd->req_packets);
	ch_bf_uint64_write(bfmt,tcp_rcd->req_bytes);
	ch_bf_uint64_write(bfmt,tcp_rcd->res_packets);
	ch_bf_uint64_write(bfmt,tcp_rcd->res_bytes);


}

static int  _tcp_record_read(ch_shm_entry_iterator_t *iter,ch_bin_format_t *bfmt,ch_shm_record_t *shm_record){

	ch_tcp_record_t *tcp_record = (ch_tcp_record_t*)shm_record;

    tcp_record->packet_type = ch_bf_uint8_read(bfmt);
    tcp_record->session_direct = ch_bf_uint8_read(bfmt);
    tcp_record->protocol_id = ch_bf_uint32_read(bfmt);
    tcp_record->session_id = ch_bf_uint64_read(bfmt);
    tcp_record->time = ch_bf_uint64_read(bfmt);
    tcp_record->src_ip = ch_bf_uint32_read(bfmt);
    tcp_record->dst_ip = ch_bf_uint32_read(bfmt);
    tcp_record->src_port = ch_bf_uint16_read(bfmt);
    tcp_record->dst_port = ch_bf_uint16_read(bfmt);
	tcp_record->req_packets = ch_bf_uint64_read(bfmt);
	tcp_record->req_bytes = ch_bf_uint64_read(bfmt);
	tcp_record->res_packets = ch_bf_uint64_read(bfmt);
	tcp_record->res_bytes = ch_bf_uint64_read(bfmt);

	return 0;
}

ch_shm_format_t * ch_shm_format_tcp_with_mmap_create(ch_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,
	uint64_t priv_data_size,int is_write){


	ch_tcp_record_t *tcp_record = (ch_tcp_record_t*)ch_pcalloc(mp,sizeof(*tcp_record));

	ch_shm_interface_t *shm_int = NULL;

	shm_int = ch_shm_mmap_create(mp,fname,fsize,entry_size,priv_data_size,is_write);
	if(shm_int == NULL){
	
		ch_log(CH_LOG_ERR,"Create a mmap file failed!");
		return NULL;
	}

	return ch_shm_format_create(mp,shm_int,entry_size,
		CH_TCP_RECORD_HEADER_SIZE,
		(ch_shm_record_t*)tcp_record,
		_tcp_record_write,
		_tcp_record_read);

}

ch_shm_format_t * ch_shm_format_tcp_with_shm_create(ch_pool_t *mp,const char *key,int proj_id,uint64_t size,
	uint64_t entry_size,uint64_t priv_data_size,int is_write){

	ch_tcp_record_t *tcp_record = (ch_tcp_record_t*)ch_pcalloc(mp,sizeof(*tcp_record));
	ch_shm_interface_t *shm_int = NULL;
	
	shm_int = ch_shm_memory_create(mp,key,proj_id,size,entry_size,priv_data_size,is_write);
	if(shm_int == NULL){
	
		ch_log(CH_LOG_ERR,"Create a shm memory failed!");
		return NULL;
	}

	return ch_shm_format_create(mp,shm_int,entry_size,
		CH_TCP_RECORD_HEADER_SIZE,
		(ch_shm_record_t*)tcp_record,
		_tcp_record_write,
		_tcp_record_read);

}

int ch_tcp_record_put(ch_shm_format_t *fmt,ch_tcp_record_t *tcp_rcd){

    int rc;
	ch_shm_record_t *rcd = (ch_shm_record_t*)tcp_rcd;

    for(;;){

        rc = ch_shm_format_put(fmt,(ch_shm_record_t*)tcp_rcd);
        if(rc == 0){
			/*ok*/
            return 0;
        }

        if(rc == -2){

            ch_log(CH_LOG_ERR,"The data is too large:%lu,discard it!",rcd->data_len);
            /*write a close packet*/
			tcp_rcd->packet_type = PACKET_TYPE_CLOSE;
			rcd->data = NULL;
			rcd->data_len = 0;
			rcd->data_offset = 0;
			rcd->record_size = CH_TCP_RECORD_HEADER_SIZE;

            
            while(ch_shm_format_put(fmt,rcd)){
                usleep(5);
            }

            return -2;
        }

        ch_log(CH_LOG_WARN,"The shm is full,sleep and try again!");

        usleep(5);
    }

	return 0;
}

