/*
 *
 *      Filename: ch_sa_arp_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-20 17:13:26
 * Last Modified: 2018-04-20 18:08:58
 */

#include "ch_sa_arp_handler.h"
#include "ch_log.h"
#include "ch_packet_record_arp.h"
#include "ch_packet_arp.h"

int ch_sa_arp_handle(ch_sa_session_task_t *sa_task,ch_packet_t *pkt){

	uint64_t time = ch_get_current_timems();
	ch_packet_record_t pkt_rcd;
	ch_packet_record_arp_t arp_rcd;
	ch_buffer_t *p_buffer = &sa_task->sa_buffer;
	size_t meta_data_size = CH_PACKET_RECORD_ARP_META_SIZE;
	size_t p_dlen = 0;

	if(ch_packet_arp_read(&arp_rcd,pkt))
		return -1;

	if(ch_buffer_need_update(p_buffer,meta_data_size)){
	
		if(ch_buffer_update(p_buffer,meta_data_size)){
		
			ch_log(CH_LOG_ERR,"Cannot create buffer failed for arp session handler!");
			return -1;
		}
	}
	
	p_dlen = ch_packet_record_arp_write(&arp_rcd,p_buffer->bdata,p_buffer->b_current_size,NULL,0);

	pkt_rcd.type = PKT_RECORD_TYPE_ARP;
	pkt_rcd.meta_data_size = meta_data_size;
	pkt_rcd.time = time;

	ch_packet_record_put(sa_task->shm_fmt,
		&pkt_rcd,
		p_buffer->bdata,
		p_dlen);

	return 0;
}

