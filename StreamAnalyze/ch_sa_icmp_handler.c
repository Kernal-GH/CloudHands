/*
 *
 *      Filename: ch_sa_icmp_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-20 17:13:26
 * Last Modified: 2018-04-20 18:01:20
 */

#include "ch_sa_icmp_handler.h"
#include "ch_log.h"
#include "ch_packet_record_icmp.h"
#include "ch_packet_icmp.h"

int ch_sa_icmp_handle(ch_sa_session_task_t *sa_task,ch_packet_t *pkt){

	uint64_t time = ch_get_current_timems();
	ch_packet_record_t pkt_rcd;
	ch_packet_record_icmp_t icmp_rcd;
	ch_buffer_t *p_buffer = &sa_task->sa_buffer;
	size_t meta_data_size = CH_PACKET_RECORD_ICMP_META_SIZE;
	size_t p_dlen = 0;
    size_t rlen = 0;

	if(ch_packet_icmp_read(&icmp_rcd,pkt))
		return -1;

    rlen = meta_data_size+icmp_rcd.dlen+sizeof(uint64_t)*2;
	if(ch_buffer_need_update(p_buffer,rlen)){
	
		if(ch_buffer_update(p_buffer,rlen)){
		
			ch_log(CH_LOG_ERR,"Cannot create buffer failed for icmp session handler!");
			return -1;
		}
	}
	
	p_dlen = ch_packet_record_icmp_write(&icmp_rcd,p_buffer->bdata,p_buffer->b_current_size,icmp_rcd.data,icmp_rcd.dlen);

	pkt_rcd.type = PKT_RECORD_TYPE_ICMP;
	pkt_rcd.meta_data_size = meta_data_size;
	pkt_rcd.time = time;

	ch_packet_record_put(sa_task->shm_fmt,
		&pkt_rcd,
		p_buffer->bdata,
		p_dlen);

	return 0;
}

