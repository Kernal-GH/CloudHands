/*
 *
 *      Filename: ch_tcp_record.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-01-29 11:43:49
 * Last Modified: 2018-07-03 11:38:10
 */

#include "ch_tcp_record.h"
#include "ch_log.h"
#include "ch_shm_mmap.h"
#include "ch_shm_memory.h"

static ch_tcp_record_t g_tcp_rcd,*g_tcp_rcd_ptr = &g_tcp_rcd;

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

static ch_shm_record_t * _tcp_record_read(ch_shm_entry_iterator_t *iter,ch_bin_format_t *bfmt){


    g_tcp_rcd_ptr->packet_type = ch_bf_uint8_read(bfmt);
    g_tcp_rcd_ptr->session_direct = ch_bf_uint8_read(bfmt);
    g_tcp_rcd_ptr->protocol_id = ch_bf_uint32_read(bfmt);
    g_tcp_rcd_ptr->session_id = ch_bf_uint64_read(bfmt);
    g_tcp_rcd_ptr->time = ch_bf_uint64_read(bfmt);
    g_tcp_rcd_ptr->src_ip = ch_bf_uint32_read(bfmt);
    g_tcp_rcd_ptr->dst_ip = ch_bf_uint32_read(bfmt);
    g_tcp_rcd_ptr->src_port = ch_bf_uint16_read(bfmt);
    g_tcp_rcd_ptr->dst_port = ch_bf_uint16_read(bfmt);
	g_tcp_rcd_ptr->req_packets = ch_bf_uint64_read(bfmt);
	g_tcp_rcd_ptr->req_bytes = ch_bf_uint64_read(bfmt);
	g_tcp_rcd_ptr->res_packets = ch_bf_uint64_read(bfmt);
	g_tcp_rcd_ptr->res_bytes = ch_bf_uint64_read(bfmt);

	return (ch_shm_record_t*)g_tcp_rcd_ptr;
}

ch_shm_format_t * ch_shm_format_tcp_with_mmap_create(ch_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,
	uint64_t priv_data_size,int is_write){


	ch_shm_interface_t *shm_int = NULL;

	shm_int = ch_shm_mmap_create(mp,fname,fsize,entry_size,priv_data_size,is_write);
	if(shm_int == NULL){
	
		ch_log(CH_LOG_ERR,"Create a mmap file failed!");
		return NULL;
	}

	return ch_shm_format_create(mp,shm_int,entry_size,
		CH_TCP_RECORD_HEADER_SIZE,
		_tcp_record_write,
		_tcp_record_read);

}

ch_shm_format_t * ch_shm_format_tcp_with_shm_create(ch_pool_t *mp,const char *key,int proj_id,uint64_t size,
	uint64_t entry_size,uint64_t priv_data_size,int is_write){

	ch_shm_interface_t *shm_int = NULL;
	
	shm_int = ch_shm_memory_create(mp,key,proj_id,size,entry_size,priv_data_size,is_write);
	if(shm_int == NULL){
	
		ch_log(CH_LOG_ERR,"Create a shm memory failed!");
		return NULL;
	}

	return ch_shm_format_create(mp,shm_int,entry_size,
		CH_TCP_RECORD_HEADER_SIZE,
		_tcp_record_write,
		_tcp_record_read);

}
