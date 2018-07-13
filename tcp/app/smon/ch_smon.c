/*
 *
 *      Filename: ch_smon.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-13 11:07:47
 * Last Modified: 2018-07-13 11:24:09
 */

#include "ch_smon.h"
#include "ch_config.h"
#include "ch_util.h"
#include "ch_tcp_record.h"
#include "ch_tcp_app_util.h"
#include "ch_log.h"
#include "ch_session_monitor.h"

typedef struct private_smon_context_t private_smon_context_t;


struct private_smon_context_t {

	ch_session_monitor_t monitor;

	const char *mmap_fname;
	
	size_t mmap_fsize;

};

static  private_smon_context_t tmp_context,*g_mcontext = &tmp_context;


#include "do_smon_context.c"

static int is_accept_by_port_for_smon(ch_tcp_app_t *app,ch_packet_tcp_t *tcp_pkt){

	private_smon_context_t *mcontext = (private_smon_context_t*)app->context;

	ch_session_monitor_item_t *item = ch_session_monitor_item_find(&mcontext->monitor,
		tcp_pkt->src_ip,tcp_pkt->dst_ip,tcp_pkt->src_port,tcp_pkt->dst_port);

	return item!=NULL;
}

static int is_accept_by_content_for_smon(ch_tcp_app_t *app ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){

    return 1;
}


static int request_content_process_for_smon(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){

	int rc;

	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_DATA,data,dlen,PROTOCOL_SMON,SESSION_DIRECT_REQ);

	rc = ch_tcp_record_put(fmt,tcp_rcd);

    return ch_trans_tcp_record_put_returnv(rc);
}

static int response_content_process_for_smon(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){

	int rc;

	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_DATA,data,dlen,PROTOCOL_SMON,SESSION_DIRECT_RES);

	rc = ch_tcp_record_put(fmt,tcp_rcd);

    return ch_trans_tcp_record_put_returnv(rc);
}

static void content_flush_for_smon(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){


	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_FLUSH,data,dlen,PROTOCOL_SMON,SESSION_DIRECT_RES);

	ch_tcp_record_put(fmt,tcp_rcd);

}

static void content_close_for_smon(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){


	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_CLOSE,data,dlen,PROTOCOL_SMON,SESSION_DIRECT_RES);

	ch_tcp_record_put(fmt,tcp_rcd);
}


static ch_tcp_app_t smon_app = {
    .context = NULL,
	.is_accept_by_port = is_accept_by_port_for_smon,
	.is_accept_by_content = is_accept_by_content_for_smon,
	.request_content_process = request_content_process_for_smon,
	.response_content_process = response_content_process_for_smon,
	.content_flush = content_flush_for_smon,
	.content_close = content_close_for_smon
};



int ch_smon_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

	if(do_smon_context_init(ta_pool->mp,g_mcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP Smon config file:%s failed!",cfname);
		return -1;
	}

	if(ch_session_monitor_load(&g_mcontext->monitor,g_mcontext->mmap_fname,g_mcontext->mmap_fsize)){
	
		ch_log(CH_LOG_ERR,"Cannot load session monitor mmapFile:%s",g_mcontext->mmap_fname);
		return -1;
	}
	
	smon_app.context = (void*)g_mcontext;

	ch_tcp_app_register(ta_pool,&smon_app);

	return 0;
}


