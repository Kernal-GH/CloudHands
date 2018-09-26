/*
 *
 *      Filename: ch_ftp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-19 10:03:29
 * Last Modified: 2018-09-26 11:41:05
 */

#include <string.h>
#include "ch_ftp.h"
#include "ch_config.h"
#include "ch_log.h"
#include "ch_list.h"
#include "ch_packet_record.h"
#include "ch_ftp_session_entry.h"
#include "ch_ftp_data_connection_pool.h"
#include "ch_tcp_app_util.h"
#include "ch_task.h"

typedef struct private_ftp_context_t private_ftp_context_t;
#define FTP_PORTS_MAX 64

struct private_ftp_context_t {

	const char *fstore_dir;
	int fstore_dir_create_type;

	uint16_t ftp_ports[FTP_PORTS_MAX];

};

static private_ftp_context_t tmp_context,*g_fcontext = &tmp_context;

#include "do_ftp_context.c"
#include "do_ftp_session_entry_create.c"
#include "do_ftp_session_parse.c"
#include "do_ftp_session_format.c"

static ch_tcp_app_t * find_by_port_for_ftp(ch_tcp_app_t *app,ch_proto_session_store_t *pstore,ch_packet_tcp_t *tcp_pkt){

	private_ftp_context_t *fcontext = (private_ftp_context_t*)app->context;

	ch_ftp_data_connection_t *ftp_dcon;

	if(ch_ports_equal(fcontext->ftp_ports,FTP_PORTS_MAX,tcp_pkt->src_port,tcp_pkt->dst_port))
		return app;

	/*Try to find ftp data session*/
	ftp_dcon = ch_ftp_data_connection_find(pstore->task_id,tcp_pkt);

	if(ftp_dcon){
	
		return ftp_dcon->ftp_data_app;
	}

	return NULL;
}

static ch_tcp_app_t* find_by_content_for_ftp(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstre ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){

    return NULL;
}

static ch_tcp_app_t ftp_app = {
    .protocol_id = PROTOCOL_FTP,
    .pkt_rcd_type =PKT_RECORD_TYPE_TCP_FTP,
    .context = NULL,
	.find_by_port = find_by_port_for_ftp ,
	.find_by_content = find_by_content_for_ftp,
    .proto_session_entry_create = do_ftp_session_entry_create,
    .proto_session_entry_clean = do_ftp_session_entry_clean,
    .proto_session_format = do_ftp_session_format,
	.request_content_parse = do_ftp_session_request_parse,
	.response_content_parse = do_ftp_session_response_parse 
};



int ch_ftp_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

	if(do_ftp_context_init(ta_pool->mp,g_fcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP ftp config file:%s failed!",cfname);
		return -1;
	}

	
	ftp_app.context = (void*)g_fcontext;

	ch_ftp_data_connection_pool_init(ta_pool->mp,g_fcontext->fstore_dir,g_fcontext->fstore_dir_create_type);

	ch_tcp_app_register(ta_pool,&ftp_app);

	return 0;
}


