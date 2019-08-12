/*
 *
 *      Filename: ch_tftp_app.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-14 11:28:42
 * Last Modified: 2018-09-14 15:02:51
 */

#include "ch_tftp_app.h"
#include "ch_log.h"
#include "ch_tftp_session.h"
#include "ch_tftp_packet.h"
#include "ch_config.h"
#include "ch_fpath.h"
#include "ch_packet_record.h"

typedef struct private_tftp_app_context_t private_tftp_app_context_t;

struct private_tftp_app_context_t {

	const char *fstore_dir;
	int fstore_dir_create_type;
	ch_fpath_t *fstore_fpath;
};

static  private_tftp_app_context_t tmp_context,*g_tcontext = &tmp_context;

#include "do_tftp_app_context.c"
#include "do_tftp_packet_parse.c"

#define TFTP_PORT_IS_MATCH(pkt_udp) ((pkt_udp)->dst_port == 69)

static ch_udp_app_session_t * _tftp_app_session_create(ch_udp_app_t *app,ch_packet_udp_t *pkt_udp){

	ch_pool_t *mp;
	ch_udp_app_session_t *app_session;

	if(!TFTP_PORT_IS_MATCH(pkt_udp))
		return NULL;

	mp = ch_pool_create(512);

	if(mp == NULL){

		ch_log(CH_LOG_ERR,"Cannot create memory pool for tftp session!");
		return NULL;
	
	}
	
	app_session = (ch_udp_app_session_t*)ch_tftp_session_create(mp,pkt_udp);

	if(app_session){
	
		app_session->app = app;
		app_session->mp = mp;
		app_session->priv_data = NULL;

	}

	return app_session;

}

static int _tftp_request_session_equal(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	return ((pkt_udp->src_ip == tftp_session->dst_ip)&&\
							 (pkt_udp->dst_ip==tftp_session->src_ip)&&\
							 (pkt_udp->dst_port==tftp_session->src_port));
}

static int _tftp_process_request_session(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp) {

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	/*change ip &port*/
	tftp_session->src_ip = pkt_udp->dst_ip;
	tftp_session->src_port = pkt_udp->dst_port;
	tftp_session->dst_ip = pkt_udp->src_ip;
	tftp_session->dst_port = pkt_udp->src_port;

	return PROCESS_REQ_SESSION_DONE;
}

static int _tftp_is_request(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){
	
	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	return tftp_session->dst_port == pkt_udp->dst_port;
}

static int _tftp_req_pkt_process(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){

	private_tftp_app_context_t *context = (private_tftp_app_context_t*)app_session->app->context;

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	return do_tftp_packet_parse(tftp_session,context,pkt_udp->pdata,pkt_udp->payload_len);

}

static int _tftp_res_pkt_process(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){

	private_tftp_app_context_t *context = (private_tftp_app_context_t*)app_session->app->context;

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	return do_tftp_packet_parse(tftp_session,context,pkt_udp->pdata,pkt_udp->payload_len);
	
}

static ssize_t _tftp_session_write(ch_udp_app_session_t *app_session,ch_data_output_t *dout){

	ssize_t rc,len = 0;

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	rc = ch_tftp_session_write(tftp_session,dout);

	if(rc == -1)
		return -1;

	return len+rc;

}

static int _tftp_session_store(ch_udp_app_session_t *app_session,ch_msgpack_store_t *dstore){

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;
	return ch_tftp_session_store(tftp_session,dstore);

}

static void _tftp_session_dump(ch_udp_app_session_t *app_session,FILE *fp){

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	ch_tftp_session_dump(tftp_session,fp);

}

static void _tftp_session_fin(ch_udp_app_session_t *app_session){

	ch_tftp_session_t *tftp_session = (ch_tftp_session_t*)app_session;

	ch_tftp_session_destroy(tftp_session);

	ch_pool_destroy(tftp_session->mp);

}

static ch_udp_app_t tftp_app = {
	.context = NULL,
	.type = PKT_RECORD_TYPE_UDP_TFTP, 
	.app_session_create = _tftp_app_session_create,
	.process_request_session = _tftp_process_request_session,
	.request_session_equal = _tftp_request_session_equal,
	.is_request = _tftp_is_request,
	.req_pkt_process = _tftp_req_pkt_process,
	.res_pkt_process = _tftp_res_pkt_process,
	.app_session_write = _tftp_session_write,
    .app_session_store = _tftp_session_store,
	.app_session_dump = _tftp_session_dump,
	.app_session_fin = _tftp_session_fin
};


int ch_tftp_app_init(ch_udp_app_pool_t *upool,const char *cfname){

	if(do_tftp_app_context_init(upool->mp,g_tcontext,cfname)){
	
		ch_log(CH_LOG_ERR,"Load UDP APP TFTP  config file:%s failed!",cfname);
		return -1;
	}

	g_tcontext->fstore_fpath = ch_fpath_create(upool->mp,g_tcontext->fstore_dir,
		g_tcontext->fstore_dir_create_type,0,0);
	
	if(g_tcontext->fstore_fpath == NULL) {
	
		ch_log(CH_LOG_ERR,"Cannot create file path for store tftp file content!");
		return -1;
	}
	
	tftp_app.context = (void*)g_tcontext;

	ch_udp_app_register(upool,&tftp_app);

	return 0;
}

