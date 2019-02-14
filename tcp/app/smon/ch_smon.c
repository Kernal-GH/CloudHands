/*
 *
 *      Filename: ch_smon.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-13 11:07:47
 * Last Modified: 2018-09-25 16:06:13
 */

#include "ch_smon.h"
#include "ch_config.h"
#include "ch_util.h"
#include "ch_tcp_record.h"
#include "ch_tcp_app_util.h"
#include "ch_log.h"
#include "ch_session_monitor.h"
#include "ch_smon_session_entry.h"
#include "ch_packet_record.h"

typedef struct private_smon_context_t private_smon_context_t;


struct private_smon_context_t {

	ch_session_monitor_t monitor;
	
    const char *req_body_dir;
	const char *res_body_dir;

	int create_dir_type;

	const char *mmap_fname;

};

static  private_smon_context_t tmp_context,*g_mcontext = &tmp_context;


#include "do_smon_context.c"
#include "do_smon_create.c"
#include "do_smon_format.c"
#include "do_smon_parse.c"

static ch_tcp_app_t* find_by_port_for_smon(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused,ch_packet_tcp_t *tcp_pkt){

	private_smon_context_t *mcontext = (private_smon_context_t*)app->context;

	ch_session_monitor_item_t *item = ch_session_monitor_item_find(&mcontext->monitor,
		tcp_pkt->src_ip,tcp_pkt->dst_ip,tcp_pkt->src_port,tcp_pkt->dst_port);

	if(item!=NULL)
		return app;

	return NULL;
}

static ch_tcp_app_t* find_by_content_for_smon(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){

    return NULL;
}


static ch_tcp_app_t smon_app = {
    .protocol_id = PROTOCOL_SMON,
    .pkt_rcd_type =PKT_RECORD_TYPE_TCP_SMON,
    .context = NULL,
	.find_by_port = find_by_port_for_smon,
	.find_by_content = find_by_content_for_smon,
    .proto_session_entry_create = do_smon_session_entry_create,
    .proto_session_entry_clean = do_smon_session_entry_clean,
    .proto_session_format = do_smon_session_format,
	.request_content_parse = do_smon_request_parse,
	.response_content_parse = do_smon_response_parse 
};



int ch_smon_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

	if(do_smon_context_init(ta_pool->mp,g_mcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP Smon config file:%s failed!",cfname);
		return -1;
	}

	if(ch_session_monitor_load(&g_mcontext->monitor,g_mcontext->mmap_fname,0)){
	
		ch_log(CH_LOG_ERR,"Cannot load session monitor mmapFile:%s",g_mcontext->mmap_fname);
		return -1;
	}
	
	smon_app.context = (void*)g_mcontext;

	ch_tcp_app_register(ta_pool,&smon_app);

	return 0;
}


