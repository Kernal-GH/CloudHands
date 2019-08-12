/*
 *
 *      Filename: ch_sa_udp_session_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 15:50:28
 * Last Modified: 2018-08-16 09:50:20
 */

#include "ch_sa_udp_session_handler.h"
#include "ch_packet_udp.h"
#include "ch_log.h"
#include "ch_atomic.h"
#include "ch_sa_udp_session.h"
#include "ch_packet_record_session_udp.h"

static ch_atomic64_t cur_session_id,*cur_session_id_ptr=&cur_session_id;

static inline const char * _get_prefix_name(ch_pool_t *mp,uint32_t tsk_id){

	return ch_psprintf(mp,"/udpsession/task%lu/rdb",(unsigned long)tsk_id);
}

static void _udp_session_out(ch_sa_udp_session_handler_t *udp_handler,
	ch_sa_udp_session_t *udp_session,
	uint8_t is_timeout,
	uint16_t timeout_tv){

	size_t p_dlen = 0;
	ch_sa_session_task_t *sa_session_task = udp_handler->session_task;
	ch_buffer_t *p_buffer = &sa_session_task->sa_buffer;

	ch_sa_data_store_t *req_dstore = udp_session->endpoint_req.dstore;
	ch_sa_data_store_t *res_dstore = udp_session->endpoint_res.dstore;
	void *req_data = NULL,*res_data = NULL;
	size_t req_dsize = 0,res_dsize = 0;

	if(req_dstore){
		req_dsize = ch_sa_data_store_size(req_dstore);
		req_data = req_dsize?req_dstore->start:NULL;
	}

	if(res_dstore){
		res_dsize = ch_sa_data_store_size(res_dstore);
		res_data = res_dsize?res_dstore->start:NULL;
	}

	size_t meta_data_size = CH_PACKET_RECORD_SESSION_UDP_META_SIZE(req_dsize,res_dsize);

	ch_packet_record_t pkt_rcd;
	ch_packet_record_session_udp_t ptmp,*p_udp_session = &ptmp;


	if(ch_buffer_need_update(p_buffer,meta_data_size)){
	
		if(ch_buffer_update(p_buffer,meta_data_size)){
		
			ch_log(CH_LOG_ERR,"Cannot create buffer failed for udp session handler!");
			ch_sa_udp_session_dstore_free(udp_session);
			return;
		}
	}

	p_udp_session->is_timeout = is_timeout;
	p_udp_session->timeout_tv = timeout_tv;
	p_udp_session->session_id = udp_session->session_id;
	
    p_udp_session->is_ipv6 = udp_session->is_ipv6;
    p_udp_session->src_addr = ch_sa_udp_session_srcaddr_get(udp_session);
    p_udp_session->dst_addr = ch_sa_udp_session_dstaddr_get(udp_session);

    p_udp_session->src_ip = ch_sa_udp_session_srcip_get(udp_session); 
	p_udp_session->dst_ip = ch_sa_udp_session_dstip_get(udp_session);

	p_udp_session->src_port = ch_sa_udp_session_srcport_get(udp_session);
	p_udp_session->dst_port = ch_sa_udp_session_dstport_get(udp_session);

	p_udp_session->req_packets = ch_sa_udp_session_req_packets(udp_session);
	p_udp_session->res_packets = ch_sa_udp_session_res_packets(udp_session);
	p_udp_session->req_bytes = ch_sa_udp_session_req_bytes(udp_session);
	p_udp_session->res_bytes = ch_sa_udp_session_res_bytes(udp_session);
	p_udp_session->req_start_time = ch_sa_udp_session_req_start_time(udp_session);
	p_udp_session->res_start_time = ch_sa_udp_session_res_start_time(udp_session);
	p_udp_session->req_last_time = ch_sa_udp_session_req_last_time(udp_session);
	p_udp_session->res_last_time = ch_sa_udp_session_res_last_time(udp_session);

	p_udp_session->req_data = req_data;
	p_udp_session->req_dsize = req_dsize;
	p_udp_session->res_data = res_data;
	p_udp_session->res_dsize = res_dsize;

	p_dlen = ch_packet_record_session_udp_write(p_udp_session,p_buffer->bdata,p_buffer->b_current_size,NULL,0);

	pkt_rcd.type = PKT_RECORD_TYPE_SESSION_UDP;
	pkt_rcd.meta_data_size = meta_data_size;
	pkt_rcd.time = ch_sa_udp_session_req_start_time(udp_session);

	ch_packet_record_put(sa_session_task->shm_fmt,
		&pkt_rcd,
		p_buffer->bdata,
		p_dlen);
	
	ch_sa_udp_session_dstore_free(udp_session);

}

static void _udp_session_timeout_cb(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data){

	ch_sa_udp_session_t *udp_session = (ch_sa_udp_session_t*)entry;

	ch_sa_udp_session_handler_t *udp_handler = (ch_sa_udp_session_handler_t*)priv_data;
	
	_udp_session_out(udp_handler,udp_session,1,tv);

}

ch_sa_udp_session_handler_t *
ch_sa_udp_session_handler_create(ch_sa_work_t *sa_work,ch_sa_session_task_t *session_task){

	ch_sa_udp_session_handler_t *udp_handler = NULL;

	udp_handler = (ch_sa_udp_session_handler_t*)ch_palloc(sa_work->mp,sizeof(*udp_handler));

	udp_handler->sa_work = sa_work;
	udp_handler->session_task = session_task;

	udp_handler->udp_pool = ch_sa_udp_session_pool_create(sa_work,_udp_session_timeout_cb,(void*)udp_handler);

	if(udp_handler->udp_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create udp sesssion pool failed for udp session handler!");
		return NULL;
	}

	ch_atomic64_init(cur_session_id_ptr);

	return udp_handler;
}

static void _udp_data_write(ch_sa_context_t *sa_context,ch_sa_udp_session_handler_t *udp_handler,ch_sa_udp_session_endpoint_t *ep,void *data,size_t dlen){

	ch_sa_data_store_pool_t *dstore_pool = udp_handler->session_task->dstore_pool;
	ch_sa_data_store_t *dstore = ep->dstore;
	size_t r_dlen = dlen;
	size_t rw_dlen = 0;

	if(dstore == NULL){
	
		dstore = ch_sa_data_store_get(dstore_pool);
		if(dstore == NULL){
			ep->error = 1;
			return;
		}
		ep->dstore = dstore;
	}

	rw_dlen = ch_sa_data_store_size(dstore);

	if(rw_dlen+dlen>sa_context->payload_data_size){
	
		r_dlen = sa_context->payload_data_size - rw_dlen;
	}

	ch_sa_data_store_write(dstore,data,r_dlen);

}

int ch_sa_udp_session_packet_handle(ch_sa_udp_session_handler_t *udp_handler,ch_packet_t *pkt){

	size_t c = 0;
	uint64_t sid;
	ch_sa_context_t *sa_context = udp_handler->sa_work->sa_context;
	uint32_t max_dlen = sa_context->payload_data_size;
	uint32_t rw_dsize = 0;

	ch_packet_udp_t udp_tmp,*pkt_udp = &udp_tmp;
	ch_sa_udp_session_t *udp_session = NULL;
	ch_sa_udp_session_endpoint_t *ep = NULL;

	if(ch_packet_udp_init_from_pkt(pkt_udp,pkt)){
	
		/*invalid udp packets*/
		return -1;
	}

	udp_session = ch_sa_udp_session_pool_entry_find(udp_handler->udp_pool,pkt_udp);
	
	if(udp_session == NULL){
	
		/*create a new udp session*/
		sid = ch_atomic64_add_return(cur_session_id_ptr,1)%ULONG_MAX;

		udp_session = ch_sa_udp_session_pool_entry_create(udp_handler->udp_pool,pkt_udp,sid);

		if(udp_session == NULL){
		
			ch_log(CH_LOG_ERR,"Create a new udp session failed!");
			return -1;
		}
		rw_dsize = 0;
		ep = &udp_session->endpoint_req;
	}else{
	
		ep = ch_sa_udp_session_endpoint_get(udp_session,pkt_udp); 
		rw_dsize = ep->dstore== NULL?0:ch_sa_data_store_size(ep->dstore);
	}

	if(ep == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Get UDP Session endpoint!");
		return -1;
	}

	ch_sa_udp_session_update(udp_session,ep,pkt_udp);

	if((pkt_udp->payload_len)>0&&(pkt_udp->pdata)&&(rw_dsize<max_dlen)&&(ep->error == 0)){
	
		_udp_data_write(sa_context,udp_handler,ep,pkt_udp->pdata,pkt_udp->payload_len);

	}
	
	c = ch_ptable_entries_timeout_free(udp_handler->udp_pool->udp_session_tbl,
		NULL);


	if(c){
	
		ch_ptable_dump(udp_handler->udp_pool->udp_session_tbl,stdout);
	}

	/*ok*/
	return 0;
}

