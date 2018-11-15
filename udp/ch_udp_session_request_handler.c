/*
 *
 *      Filename: ch_udp_session_request_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-12 18:02:09
 * Last Modified: 2018-09-13 17:22:43
 */

#include "ch_udp_session_request_handler.h"
#include "ch_udp_session_handler.h"
#include "ch_packet_udp.h"
#include "ch_atomic.h"
#include "ch_udp_session.h"
#include "ch_log.h"

static ch_atomic64_t cur_session_id,*cur_session_id_ptr=&cur_session_id;

static void _udp_session_request_timeout_cb(ch_plist_entry_t *entry,uint64_t tv ch_unused,void *priv_data ch_unused){

	ch_udp_session_request_t *req_session = (ch_udp_session_request_t*)entry;

	ch_udp_app_session_fin(req_session->app_session);

}

ch_udp_session_request_handler_t *
ch_udp_session_request_handler_create(ch_udp_work_t *udp_work,ch_udp_session_task_t *session_task){

	ch_udp_session_request_handler_t *req_handler = NULL;

	req_handler = (ch_udp_session_request_handler_t*)ch_palloc(udp_work->mp,sizeof(*req_handler));

	req_handler->udp_work = udp_work;
	req_handler->session_task = session_task;

	req_handler->req_pool = ch_udp_session_request_pool_create(udp_work,_udp_session_request_timeout_cb,(void*)req_handler);

	if(req_handler->req_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create udp sesssion request pool failed for udp session request handler!");
		return NULL;
	}

	ch_atomic64_init(cur_session_id_ptr);
	
	return req_handler;

}

static int _create_udp_session_and_process(ch_udp_session_request_handler_t *req_handler,
	ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){

	uint64_t sid = ch_atomic64_add_return(cur_session_id_ptr,1)%ULONG_MAX;
	ch_udp_session_t *udp_session;

	udp_session = ch_udp_session_pool_entry_create(CH_UDP_SESSION_POOL_GET(req_handler),pkt_udp,app_session,sid);
	if(udp_session == NULL){	
		ch_log(CH_LOG_ERR,"Create a new udp session failed!");
		return -1;
	}

	return ch_udp_session_packet_handle(req_handler->session_task->udp_session_handler,udp_session,pkt_udp,1);
}

int
ch_udp_session_request_packet_handle(ch_udp_session_request_handler_t *req_handler,ch_packet_t *pkt){

	int rc;

	ch_packet_udp_t udp_tmp,*pkt_udp = &udp_tmp;
	ch_udp_app_pool_t *app_pool = req_handler->udp_work->app_pool;
	ch_udp_session_t *udp_session = NULL; 
	ch_udp_session_request_t *req_session;
	ch_udp_app_session_t *app_session;

	if(ch_packet_udp_init_from_pkt(pkt_udp,pkt)){
	
		/*invalid udp packets*/
		return -1;
	}

	/*Try to find udp session */
	udp_session = ch_udp_session_pool_entry_find(CH_UDP_SESSION_POOL_GET(req_handler),
		pkt_udp);

	if(udp_session){
		/*udp session is existed,then udp session handler process it */
		return ch_udp_session_packet_handle(req_handler->session_task->udp_session_handler,udp_session,pkt_udp,0);
	}
	
	/*if udp session is not existed
	 *,Try to find udp session request
	 * */
	req_session = ch_udp_session_request_pool_entry_find(req_handler->req_pool,pkt_udp);
	if(req_session){

		/*process request sesssion*/
		rc = ch_udp_app_session_request_process(req_session,req_session->app_session,pkt_udp); 
		if(rc ==  PROCESS_REQ_SESSION_CONTINUE)
			return 0;
		
		if(rc == PROCESS_REQ_SESSION_DONE)
		{
			_create_udp_session_and_process(req_handler,req_session->app_session,pkt_udp);
		}

		/*ERR OR DONE*/
		ch_udp_session_request_pool_entry_free(req_handler->req_pool,req_session);

	}else{
	
		/*udp session and request session are not existed,
		 * 1: try to create app session
		 * if a app session can create then create a udp session request,
		 * else discard this packet,because no application need this packet!*/
		app_session = ch_udp_app_session_create(app_pool,pkt_udp);
		if(app_session == NULL){
			return -1;
		}
		/*if app need process session request*/
		if(app_session->app->process_request_session){
		
			req_session = ch_udp_session_request_pool_entry_create(req_handler->req_pool,pkt_udp,app_session);
			if(req_session == NULL){

				ch_log(CH_LOG_ERR,"Cannot create a udp request session!");
				return -1;
			}

		}else{
		
			/*not need process a session request,then create a udp session and process it directly*/
			
			return _create_udp_session_and_process(req_handler,app_session,pkt_udp);

		}
	}

	return 0;
}


