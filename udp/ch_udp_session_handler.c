/*
 *
 *      Filename: ch_udp_session_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 15:50:28
 * Last Modified: 2018-06-20 19:08:34
 */

#include "ch_udp_session_handler.h"
#include "ch_packet_udp.h"
#include "ch_log.h"
#include "ch_atomic.h"
#include "ch_udp_session.h"
#include "ch_packet_record.h"

static ch_atomic64_t cur_session_id,*cur_session_id_ptr=&cur_session_id;

static void _udp_session_out(ch_udp_session_handler_t *udp_handler,
	ch_udp_session_t *udp_session,
	uint8_t is_timeout ch_unused,
	uint16_t timeout_tv ch_unused){

	size_t dlen = 0;
	void *data;

	ch_udp_app_t *udp_app = udp_session->app;

	ch_udp_session_task_t *udp_session_task = udp_handler->session_task;
	ch_data_output_t *dout = &udp_session_task->dout;
	CH_DOUT_RESET(dout);

	ch_packet_record_t pkt_rcd;

	//ch_udp_app_session_dump(udp_app,stdout,udp_session,NULL);

	if(-1 == ch_udp_app_session_write(udp_app,dout,udp_session,NULL))
	{
	
		ch_log(CH_LOG_ERR,"Write UDP APP Data Failed!");
		return;
	}
	
	dlen = CH_DOUT_CONTENT_SIZE(dout);
	data = CH_DOUT_CONTENT(dout);

	pkt_rcd.type = udp_app->type;
	pkt_rcd.meta_data_size = 0;
	pkt_rcd.time = ch_udp_session_req_start_time(udp_session);

	ch_packet_record_put(udp_session_task->shm_fmt,
		&pkt_rcd,
		data,
		dlen);

}

static void _udp_session_timeout_cb(ch_ptable_entry_t *entry,uint64_t tv,void *priv_data){

	ch_udp_session_t *udp_session = (ch_udp_session_t*)entry;

	ch_udp_session_handler_t *udp_handler = (ch_udp_session_handler_t*)priv_data;
	
	_udp_session_out(udp_handler,udp_session,1,tv);
	
	ch_udp_app_session_fin(udp_session->app,udp_session,NULL);

}

ch_udp_session_handler_t *
ch_udp_session_handler_create(ch_udp_work_t *udp_work,ch_udp_session_task_t *session_task){


	ch_udp_session_handler_t *udp_handler = NULL;

	udp_handler = (ch_udp_session_handler_t*)ch_palloc(udp_work->mp,sizeof(*udp_handler));

	udp_handler->udp_work = udp_work;
	udp_handler->session_task = session_task;

	udp_handler->udp_pool = ch_udp_session_pool_create(udp_work,_udp_session_timeout_cb,(void*)udp_handler);

	if(udp_handler->udp_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create udp sesssion pool failed for udp session handler!");
		return NULL;
	}

	
	ch_atomic64_init(cur_session_id_ptr);

	return udp_handler;
}

static void _udp_app_pkt_handle(ch_udp_session_handler_t *udp_session_handler,
	ch_udp_session_t *udp_session,
	ch_udp_app_t *app,
	ch_packet_udp_t *pkt_udp){


	int rc;

	rc = app->pkt_process(udp_session->app_session,pkt_udp,NULL);
	if(rc == PROCESS_CONTINUE)
		return;

	if(rc == PROCESS_DONE){
		_udp_session_out(udp_session_handler,udp_session,0,0);
	}

	ch_udp_app_session_fin(app,udp_session,NULL);
	ch_udp_session_pool_entry_free(udp_session_handler->udp_pool,udp_session);
}

int ch_udp_session_packet_handle(ch_udp_session_handler_t *udp_handler,ch_packet_t *pkt){

	uint64_t sid;
	size_t c;

	ch_packet_udp_t udp_tmp,*pkt_udp = &udp_tmp;
	ch_udp_session_t *udp_session = NULL;
	ch_udp_session_endpoint_t *ep = NULL;
	ch_udp_app_pool_t *app_pool = udp_handler->udp_work->app_pool;
	ch_udp_app_t *udp_app;

	if(ch_packet_udp_init_from_pkt(pkt_udp,pkt)){
	
		/*invalid udp packets*/
		return -1;
	}

	udp_session = ch_udp_session_pool_entry_find(udp_handler->udp_pool,pkt_udp);
	
	if(udp_session == NULL){

		udp_app = ch_udp_app_find(app_pool,pkt_udp);
		if(udp_app == NULL){
		
			return -1;
		}
		if(!ch_udp_app_session_is_request(udp_app,pkt_udp))
		{
		
			/*no request !*/
			return -1;
		}

		/*create a new udp session*/
		sid = ch_atomic64_add_return(cur_session_id_ptr,1)%ULONG_MAX;

		udp_session = ch_udp_session_pool_entry_create(udp_handler->udp_pool,pkt_udp,udp_app,sid);

		if(udp_session == NULL){
		
			ch_log(CH_LOG_ERR,"Create a new udp session failed!");
			return -1;
		}

		ep = &udp_session->endpoint_req;
	}else{

		udp_app = udp_session->app;
		ep = ch_udp_session_endpoint_get(udp_session,pkt_udp); 
	}

	if(ep == NULL || udp_app == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Get UDP Session endpoint or udp app!");
		return -1;
	}

	ch_udp_session_update(udp_session,ep,pkt_udp);
	
	_udp_app_pkt_handle(udp_handler,udp_session,udp_app,pkt_udp);

	
	c = ch_ptable_entries_timeout_free(udp_handler->udp_pool->udp_session_tbl,
		NULL);

	if(c){
	
		ch_ptable_dump(udp_handler->udp_pool->udp_session_tbl,stdout);
	}
	/*ok*/
	return 0;
}

