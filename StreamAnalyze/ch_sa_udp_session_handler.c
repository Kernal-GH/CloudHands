/*
 *
 *      Filename: ch_sa_udp_session_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 15:50:28
 * Last Modified: 2018-04-21 11:12:16
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

	char *dbname;

	size_t p_dlen = 0,db_path_len = 0;

	ch_sa_session_task_t *sa_session_task = udp_handler->session_task;
	ch_buffer_t *p_buffer = &sa_session_task->sa_buffer;

	dbname = udp_session->rdb_store->rdb_name;
	db_path_len = strlen(dbname);

	ch_rdb_store_ref_update(udp_session->rdb_store,-1);

	size_t meta_data_size = CH_PACKET_RECORD_SESSION_UDP_META_SIZE(db_path_len);

	ch_packet_record_t pkt_rcd;
	ch_packet_record_session_udp_t ptmp,*p_udp_session = &ptmp;


	if(ch_buffer_need_update(p_buffer,meta_data_size)){
	
		if(ch_buffer_update(p_buffer,meta_data_size)){
		
			ch_log(CH_LOG_ERR,"Cannot create buffer failed for udp session handler!");
			return;
		}
	}

	p_udp_session->is_timeout = is_timeout;
	p_udp_session->timeout_tv = timeout_tv;
	p_udp_session->session_id = udp_session->session_id;
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

	p_udp_session->req_data_size = ch_sa_udp_session_req_data_size(udp_session);
	p_udp_session->res_data_size = ch_sa_udp_session_res_data_size(udp_session);
	p_udp_session->req_data_seq = ch_sa_udp_session_req_data_seq(udp_session);
	p_udp_session->res_data_seq = ch_sa_udp_session_res_data_seq(udp_session);


	p_udp_session->db_path_len = db_path_len;

	p_udp_session->db_path = dbname;
	
	p_dlen = ch_packet_record_session_udp_write(p_udp_session,p_buffer->bdata,p_buffer->b_current_size,NULL,0);

	pkt_rcd.type = PKT_RECORD_TYPE_SESSION_UDP;
	pkt_rcd.meta_data_size = meta_data_size;
	pkt_rcd.time = ch_sa_udp_session_req_start_time(udp_session);

	ch_packet_record_put(sa_session_task->shm_fmt,
		&pkt_rcd,
		p_buffer->bdata,
		p_dlen);

}

static void _udp_session_timeout_cb(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data){

	ch_sa_udp_session_t *udp_session = (ch_sa_udp_session_t*)entry;

	ch_sa_udp_session_handler_t *udp_handler = (ch_sa_udp_session_handler_t*)priv_data;
	
	_udp_session_out(udp_handler,udp_session,1,tv);

}

ch_sa_udp_session_handler_t *
ch_sa_udp_session_handler_create(ch_sa_work_t *sa_work,ch_sa_session_task_t *session_task){

	ch_sa_context_t *sa_context = sa_work->sa_context;

	ch_sa_udp_session_handler_t *udp_handler = NULL;

	udp_handler = (ch_sa_udp_session_handler_t*)ch_palloc(sa_work->mp,sizeof(*udp_handler));

	udp_handler->sa_work = sa_work;
	udp_handler->session_task = session_task;

	udp_handler->udp_pool = ch_sa_udp_session_pool_create(sa_work,_udp_session_timeout_cb,(void*)udp_handler);

	if(udp_handler->udp_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create udp sesssion pool failed for udp session handler!");
		return NULL;
	}

	
	/*create udp rdb store*/
	udp_handler->rdb_store_pool = ch_rdb_store_pool_create(sa_work->mp,sa_context->rdb_store_dir,
		_get_prefix_name(sa_work->mp,session_task->task.tsk_id),
		sa_context->rdb_name_create_type,
		sa_context->rdb_using_timeout);

	if(udp_handler->rdb_store_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create rdb pool failed for udp session handler!");
		return NULL;

	}
	
	ch_atomic64_init(cur_session_id_ptr);

	return udp_handler;
}

static void _udp_data_write(ch_sa_context_t *sa_context,
	ch_sa_udp_session_t *udp_session,ch_sa_udp_session_endpoint_t *ep,void *data,size_t dlen){

	char key[128] = {0};
	size_t r_dlen = dlen;
	const char *tstr = ep==&udp_session->endpoint_req?"REQ":"RES";
	
	if(ep->data_size+dlen>sa_context->payload_data_size){
	
		r_dlen = sa_context->payload_data_size - ep->data_size;
	}

	snprintf(key,128,"%lu%s",udp_session->session_id,tstr);

	ep->data_size += r_dlen;
	ep->data_seq += 1;


	ch_rdb_store_put(udp_session->rdb_store,key,strlen(key),(const char*)data,r_dlen);

}


int ch_sa_udp_session_packet_handle(ch_sa_udp_session_handler_t *udp_handler,ch_packet_t *pkt){

	size_t c = 0;
	uint64_t sid;
	ch_sa_context_t *sa_context = udp_handler->sa_work->sa_context;
	uint32_t max_dlen = sa_context->payload_data_size;

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

		udp_session->rdb_store = ch_rdb_store_pool_get(udp_handler->rdb_store_pool);

		ep = &udp_session->endpoint_req;
	}else{
	
		ep = ch_sa_udp_session_endpoint_get(udp_session,pkt_udp); 
	}

	if(ep == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Get UDP Session endpoint!");
		return -1;
	}

	ch_sa_udp_session_update(udp_session,ep,pkt_udp);

	if((pkt_udp->payload_len)>0&&(pkt_udp->pdata)&&(ep->data_size<max_dlen)){
	
		_udp_data_write(sa_context,udp_session,ep,pkt_udp->pdata,pkt_udp->payload_len);

	}
	
	c = ch_ptable_entries_timeout_free(udp_handler->udp_pool->udp_session_tbl,
		NULL);


	if(c){
	
		//ch_ptable_dump(shandler->tcp_session_pool->tcp_session_tbl,stdout);
		
		ch_rdb_store_pool_dump(udp_handler->rdb_store_pool,stdout);

	}

	ch_rdb_store_pool_check(udp_handler->rdb_store_pool);

	/*ok*/
	return 0;
}

