/*
 *
 *      Filename: ch_udp_session_handler.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 15:50:28
 * Last Modified: 2018-09-13 17:05:40
 */

#include "ch_udp_session_handler.h"
#include "ch_packet_udp.h"
#include "ch_log.h"
#include "ch_udp_session.h"
#include "ch_packet_record.h"

static void _udp_session_out(ch_udp_session_handler_t *udp_handler,
	ch_udp_session_t *udp_session,
	uint8_t is_timeout ch_unused,
	uint16_t timeout_tv ch_unused){

	
    ch_packet_record_t pkt_rcd;
	size_t dlen = 0;
	void *data;

	ch_udp_session_task_t *udp_session_task = udp_handler->session_task;
    ch_udp_context_t *ucontext = udp_session_task->udp_work->udp_context;

    if(ucontext->use_msgpack){

        ch_msgpack_store_t *dstore = udp_session_task->dstore;
        ch_msgpack_store_reset(dstore);
        
        if(ch_udp_app_session_store(udp_session,udp_session->app_session,dstore)<0)
        {
        
            ch_log(CH_LOG_ERR,"Write UDP APP msgpack Data Failed!");
            return;
        }

        data = dstore->pk_buf.data;
        dlen = dstore->pk_buf.size;

    }else{
        
        ch_data_output_t *dout = &udp_session_task->dout;
        CH_DOUT_RESET(dout);
        
        if(ch_udp_app_session_write(udp_session,udp_session->app_session,dout)<0)
        {
        
            ch_log(CH_LOG_ERR,"Write UDP APP Data Failed!");
            return;
        }
        
        dlen = CH_DOUT_CONTENT_SIZE(dout);
        data = CH_DOUT_CONTENT(dout);
    }

    if(data==NULL||dlen==0)
        return;

	pkt_rcd.type = udp_session->app_session->app->type;
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
	
	ch_udp_app_session_fin(udp_session->app_session);

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

	return udp_handler;
}

static void _udp_app_pkt_handle(ch_udp_session_handler_t *udp_session_handler,
	ch_udp_session_t *udp_session,
	ch_packet_udp_t *pkt_udp){


	int rc;

	rc = ch_udp_app_session_packet_process(udp_session->app_session,pkt_udp);
	if(rc == PROCESS_CONTINUE)
		return;

	if(rc == PROCESS_DONE){
		_udp_session_out(udp_session_handler,udp_session,0,0);
	}

	ch_udp_app_session_fin(udp_session->app_session);

	ch_udp_session_pool_entry_free(udp_session_handler->udp_pool,udp_session);
}

int ch_udp_session_packet_handle(ch_udp_session_handler_t *udp_handler,ch_udp_session_t *udp_session,
	ch_packet_udp_t *pkt_udp,int is_new_created){

	ch_udp_session_endpoint_t *ep = NULL;

	if(is_new_created){
		ep = &udp_session->endpoint_req;
	}else{
		ep = ch_udp_session_endpoint_get(udp_session,pkt_udp); 
	}

	if(ep == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot Get UDP Session endpoint!");
		return -1;
	}

	ch_udp_session_update(udp_session,ep,pkt_udp);
	
	_udp_app_pkt_handle(udp_handler,udp_session,pkt_udp);

	/*ok*/
	return 0;
}

