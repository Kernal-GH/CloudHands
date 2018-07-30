/*
 *
 *      Filename: ch_http.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 16:21:25
 * Last Modified: 2018-07-30 09:59:54
 */

#include "ch_http.h"
#include "ch_config.h"
#include "ch_util.h"
#include "ch_tcp_app_util.h"
#include "ch_log.h"
#include "ch_list.h"
#include "ch_http_session.h"
#include "ch_tcp_app_pool.h"
#include "ch_http_session_entry.h"
#include "ch_packet_record.h"

typedef struct private_http_context_t private_http_context_t;

#define HTTP_PORTS_MAX 64

struct private_http_context_t {

	const char *req_body_dir;
	const char *res_body_dir;


	int create_dir_type;

	uint16_t http_ports[HTTP_PORTS_MAX];

};

static  private_http_context_t tmp_context,*g_hcontext = &tmp_context;


#include "do_http_context.c"
#include "do_http_create.c"
#include "do_http_format.c"
#include "do_http_parse.c"

static int is_accept_by_port_for_http(ch_tcp_app_t *app,ch_packet_tcp_t *tcp_pkt){

	private_http_context_t *hcontext = (private_http_context_t*)app->context;


    return ch_ports_equal(hcontext->http_ports,HTTP_PORTS_MAX,tcp_pkt->src_port,tcp_pkt->dst_port);

}

static int is_accept_by_content_for_http(ch_tcp_app_t *app ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){

    return 1;
}


static ch_tcp_app_t http_app = {
    .protocol_id = PROTOCOL_HTTP ,
    .pkt_rcd_type = PKT_RECORD_TYPE_TCP_HTTP,
    .context = NULL,
	.is_accept_by_port = is_accept_by_port_for_http,
	.is_accept_by_content = is_accept_by_content_for_http,
    .proto_session_entry_create = do_http_session_entry_create,
    .proto_session_entry_clean = do_http_session_entry_clean,
    .proto_session_format = do_http_session_format,
	.request_content_parse = do_http_request_parse,
	.response_content_parse = do_http_response_parse 
};



int ch_http_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

    memset(g_hcontext->http_ports,0,HTTP_PORTS_MAX);
	if(do_http_context_init(ta_pool->mp,g_hcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP Http config file:%s failed!",cfname);
		return -1;
	}

	http_app.context = (void*)g_hcontext;

	ch_tcp_app_register(ta_pool,&http_app);

	return 0;
}

