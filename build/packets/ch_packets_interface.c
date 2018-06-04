/*
 *
 *      Filename: ch_packets_interface.c
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-05-31 11:50:03
 * Last Modified: 2017-06-02 17:05:14
 */

#include "ch_log.h"
#include "ch_packets_interface.h"

ch_packets_interface_t * ch_packets_interface_create(apr_pool_t *mp,const char *cfname){

	ch_packets_interface_t *pin = (ch_packets_interface_t*)apr_palloc(mp,sizeof(*pin));
	ch_packets_context_t *pcontext = (ch_packets_context_t*)apr_palloc(mp,sizeof(*pcontext));

	pin->pcontext = pcontext;
	pcontext->mp = mp;

    rte_spinlock_init(&pin->spin_lock);
	
	ch_packets_context_init(pcontext,cfname);

	pin->pfmt = ch_mmap_packets_format_create(mp,pcontext->mmap_file_dir,
		pcontext->mmap_file_size,
		pcontext->mmap_file_entry_size,1);

	if(pin->pfmt == NULL){
	
		ch_log(CH_LOG_ERR,"Create packets mmap file format instance failed!");
		return NULL;
	}

	return pin;
}

static inline void tcp_packet_entry_init(ch_packets_interface_t *pin,ch_mmap_packets_entry_t *pentry,ch_assemble_packet_tcp_t *tp){

	size_t dlen = CH_MIN(tp->payload_len,pin->pcontext->max_packet_data_size);

	pentry->entry_size = dlen+CH_MMAP_PACKETS_ENTRY_HEADER_LEN;
	pentry->data_len = dlen;
	pentry->data = tp->pdata;
	pentry->flags = tp->tcp_flags;
	pentry->sent_seq = tp->sent_seq;
	pentry->recv_ack = tp->recv_ack;
	pentry->p_size = tp->payload_len;

}

static inline void udp_packet_entry_init(ch_packets_interface_t *pin,ch_mmap_packets_entry_t *pentry,ch_assemble_packet_udp_t *up){

	size_t dlen = CH_MIN(up->payload_len,pin->pcontext->max_packet_data_size);

	pentry->entry_size = dlen+CH_MMAP_PACKETS_ENTRY_HEADER_LEN;

	pentry->data_len = dlen;
	pentry->data = up->pdata;
	pentry->p_size = up->payload_len;
}

static inline int
packet_entry_init(ch_packets_interface_t *pin,ch_mmap_packets_entry_t *pentry,ch_assemble_packet_t *p,int proto_id){

	pentry->magic = PFIMAGIC;
	pentry->entry_size = 0;
	pentry->proto_id = proto_id;
	pentry->flags = 0;
	pentry->sent_seq = 0;
	pentry->recv_ack = 0;
	pentry->time = ch_get_current_timems();
	pentry->src_ip = p->src_ip;
	pentry->dst_ip = p->dst_ip;
	pentry->src_port = p->src_port;
	pentry->dst_port = p->dst_port;
	pentry->data_len = 0;
	pentry->data = NULL;

	switch(proto_id){

	case PROTO_TCP:
		tcp_packet_entry_init(pin,pentry,(ch_assemble_packet_tcp_t*)p);
		break;

	case PROTO_UDP:
		udp_packet_entry_init(pin,pentry,(ch_assemble_packet_udp_t*)p);
		break;
	
	default:
		ch_log(CH_LOG_ERR,"invalid packets,discard it .............................");
		return -1;
	}

	return 0;
}

void ch_packets_interface_put(ch_packets_interface_t *pin,ch_assemble_packet_t *p,int proto_id){


	ch_mmap_packets_entry_t pentry;

	int rc = packet_entry_init(pin,&pentry,p,proto_id);

	if(rc == 0){
	
		ch_pin_lock(pin);
		ch_mmap_packets_format_put(pin->pfmt,&pentry);
		ch_pin_unlock(pin);
	}
}
