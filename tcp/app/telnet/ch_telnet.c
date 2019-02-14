/*
 * =====================================================================================
 *
 *       Filename:  ch_telnet.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/22/2018 08:27:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_telnet.h"
#include "ch_config.h"
#include "ch_util.h"
#include "ch_tcp_record.h"
#include "ch_tcp_app_util.h"
#include "ch_log.h"
#include "ch_telnet_session_entry.h"
#include "ch_packet_record.h"

#define TELNET_PORTS_MAX 64

typedef struct private_telnet_context_t private_telnet_context_t;


struct private_telnet_context_t {

    const char *content_dir;
	int create_dir_type;

	uint16_t telnet_ports[TELNET_PORTS_MAX];

};

static  private_telnet_context_t tmp_context,*g_mcontext = &tmp_context;


#include "do_telnet_context.c"
#include "do_telnet_create.c"
#include "do_telnet_format.c"
#include "do_telnet_parse.c"

static ch_tcp_app_t* find_by_port_for_telnet(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused,ch_packet_tcp_t *tcp_pkt){

	private_telnet_context_t *mcontext = (private_telnet_context_t*)app->context;

	if(ch_ports_equal(mcontext->telnet_ports,TELNET_PORTS_MAX,tcp_pkt->src_port,tcp_pkt->dst_port))
		return app;

    return NULL;
}


static ch_tcp_app_t* find_by_content_for_telnet(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstore ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){

    return NULL;
}


static ch_tcp_app_t telnet_app = {
    
    .protocol_id = PROTOCOL_TELNET,
    .pkt_rcd_type =PKT_RECORD_TYPE_TCP_TELNET,
    .context = NULL,
	.find_by_port = find_by_port_for_telnet,
	.find_by_content = find_by_content_for_telnet,
    .proto_session_entry_create = do_telnet_session_entry_create,
    .proto_session_entry_clean = do_telnet_session_entry_clean,
    .proto_session_format = do_telnet_session_format,
	.request_content_parse = do_telnet_request_parse,
	.response_content_parse = do_telnet_response_parse 
};



int ch_telnet_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

	if(do_telnet_context_init(ta_pool->mp,g_mcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP telnet config file:%s failed!",cfname);
		return -1;
	}

	
	telnet_app.context = (void*)g_mcontext;

	ch_tcp_app_register(ta_pool,&telnet_app);

	return 0;
}


