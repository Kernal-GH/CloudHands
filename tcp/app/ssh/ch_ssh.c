/*
 *
 *      Filename: ch_ssh.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-19 10:03:29
 * Last Modified: 2018-09-26 11:41:05
 */

#include "ch_ssh.h"
#include "ch_config.h"
#include "ch_log.h"
#include "ch_packet_record.h"
#include "ch_ssh_session_entry.h"
#include "ch_tcp_app_util.h"

typedef struct private_ssh_context_t private_ssh_context_t;
#define SSH_PORTS_MAX 64

struct private_ssh_context_t {

	uint16_t ssh_ports[SSH_PORTS_MAX];

};

static private_ssh_context_t tmp_ssh_context,*g_ssh_context = &tmp_ssh_context;

#include "do_ssh_context.c"
#include "do_ssh_create.c"
#include "do_ssh_parse.c"
#include "do_ssh_format.c"

static ch_tcp_app_t * find_by_port_for_ssh(ch_tcp_app_t *app,ch_proto_session_store_t *pstore ch_unused,ch_packet_tcp_t *tcp_pkt){

	private_ssh_context_t *ssh_context = (private_ssh_context_t*)app->context;

	if(ch_ports_equal(ssh_context->ssh_ports,SSH_PORTS_MAX,tcp_pkt->src_port,tcp_pkt->dst_port))
		return app;

	return NULL;
}

static ch_tcp_app_t* find_by_content_for_ssh(ch_tcp_app_t *app ch_unused,ch_proto_session_store_t *pstre ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){

    return NULL;
}

static ch_tcp_app_t ssh_app = {
    .protocol_id = PROTOCOL_SSH,
    .pkt_rcd_type =PKT_RECORD_TYPE_TCP_SSH,
    .context = NULL,
	.find_by_port = find_by_port_for_ssh ,
	.find_by_content = find_by_content_for_ssh,
    .proto_session_entry_create = do_ssh_session_entry_create,
    .proto_session_entry_clean = do_ssh_session_entry_clean,
    .proto_session_format = do_ssh_session_format,
	.request_content_parse = do_ssh_request_parse,
	.response_content_parse = do_ssh_response_parse 
};



int ch_ssh_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

	if(do_ssh_context_init(ta_pool->mp,g_ssh_context,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP ssh config file:%s failed!",cfname);
		return -1;
	}


	ssh_app.context = (void*)g_ssh_context;


	ch_tcp_app_register(ta_pool,&ssh_app);

	return 0;
}


