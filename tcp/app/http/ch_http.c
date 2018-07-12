/*
 *
 *      Filename: ch_http.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 16:21:25
 * Last Modified: 2018-07-12 19:26:59
 */

#include "ch_http.h"
#include "ch_config.h"
#include "ch_util.h"
#include "ch_tcp_record.h"
#include "ch_tcp_app_util.h"
#include "ch_log.h"

typedef struct private_http_context_t private_http_context_t;

#define HTTP_PORTS_MAX 64

struct private_http_context_t {

	uint16_t http_ports[HTTP_PORTS_MAX];

};

static  private_http_context_t tmp_context,*g_hcontext = &tmp_context;


#include "do_http_context.c"

static int is_accept_by_port_for_http(ch_tcp_app_t *app,ch_packet_tcp_t *tcp_pkt){

	private_http_context_t *hcontext = (private_http_context_t*)app->context;


    return ch_ports_equal(hcontext->http_ports,HTTP_PORTS_MAX,tcp_pkt->src_port,tcp_pkt->dst_port);

}

static int is_accept_by_content_for_http(ch_tcp_app_t *app ch_unused,ch_packet_tcp_t *tcp_pkt ch_unused,
	void *data ch_unused,size_t dlen ch_unused){

    return 1;
}


static int request_content_process_for_http(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){

	int rc;

	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_DATA,data,dlen,PROTOCOL_HTTP,SESSION_DIRECT_REQ);

	rc = ch_tcp_record_put(fmt,tcp_rcd);

    return ch_trans_tcp_record_put_returnv(rc);
}

static int response_content_process_for_http(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){

	int rc;

	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_DATA,data,dlen,PROTOCOL_HTTP,SESSION_DIRECT_RES);

	rc = ch_tcp_record_put(fmt,tcp_rcd);

    return ch_trans_tcp_record_put_returnv(rc);
}

static void content_flush_for_http(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){


	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_FLUSH,data,dlen,PROTOCOL_HTTP,SESSION_DIRECT_RES);

	ch_tcp_record_put(fmt,tcp_rcd);

}

static void content_close_for_http(ch_tcp_app_t *app ch_unused,ch_shm_format_t *fmt,
	ch_tcp_session_t *tsession,void *data,size_t dlen){


	ch_tcp_record_t tmp_rcd,*tcp_rcd = &tmp_rcd;

	ch_tcp_record_init(tcp_rcd,tsession,PACKET_TYPE_CLOSE,data,dlen,PROTOCOL_HTTP,SESSION_DIRECT_RES);

	ch_tcp_record_put(fmt,tcp_rcd);
}


static ch_tcp_app_t http_app = {
    .context = NULL,
	.is_accept_by_port = is_accept_by_port_for_http,
	.is_accept_by_content = is_accept_by_content_for_http,
	.request_content_process = request_content_process_for_http,
	.response_content_process = response_content_process_for_http,
	.content_flush = content_flush_for_http,
	.content_close = content_close_for_http
};



int ch_http_init(ch_tcp_app_pool_t *ta_pool,const char *cfname){

	if(do_http_context_init(ta_pool->mp,g_hcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load TCP APP Http config file:%s failed!",cfname);
		return -1;
	}

	http_app.context = (void*)g_hcontext;

	ch_tcp_app_register(ta_pool,&http_app);

	return 0;
}

