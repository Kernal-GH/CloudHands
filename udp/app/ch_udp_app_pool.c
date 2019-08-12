/*
 *
 *      Filename: ch_udp_app_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 15:49:46
 * Last Modified: 2018-09-14 15:11:01
 */

#include "ch_udp_app_pool.h"
#include "ch_log.h"
#include "ch_net_util.h"
#include "ch_dns_app.h"
#include "ch_tftp_app.h"
#include "ch_smon_app.h"

#define process_register_retv(rc,proto) do { \
	if(rc){\
		ch_log(CH_LOG_ERR,"register proto:%s failed!",proto);\
		return -1;\
	}\
}while(0)

static int _register_all_apps(ch_udp_app_pool_t *upool,ch_udp_app_context_t *ucontext){

	int rc;

	if(ucontext->dns_is_on){
		rc = ch_dns_app_init(upool,ucontext->dns_cfname);
		process_register_retv(rc,"dns");
	}
	
	if(ucontext->tftp_is_on){
		rc = ch_tftp_app_init(upool,ucontext->tftp_cfname);
		process_register_retv(rc,"tftp");
	}

	if(ucontext->smon_is_on){
		rc = ch_smon_app_init(upool,ucontext->smon_cfname);
		process_register_retv(rc,"smon");
	}

    return 0;
}

ch_udp_app_pool_t * ch_udp_app_pool_create(ch_pool_t *mp,const char *cfname){

	ch_udp_app_pool_t *upool = ch_pcalloc(mp,sizeof(*upool));
	upool->mp = mp;

	upool->ucontext = ch_udp_app_context_create(mp,cfname);
	if(upool->ucontext == NULL){
	
		ch_log(CH_LOG_ERR,"Create udp context failed for udp app pool!");
		return NULL;
	}

	upool->apps = ch_array_make(mp,16,sizeof(ch_udp_app_t*));

	if(_register_all_apps(upool,upool->ucontext)){
	
		return NULL;
	}

	return upool;
}


ch_udp_app_session_t* ch_udp_app_session_create(ch_udp_app_pool_t *app_pool,ch_packet_udp_t *pkt_udp){

	ch_udp_app_session_t *app_session = NULL;

	int i;
	ch_udp_app_t **apps,*app = NULL;

	apps = (ch_udp_app_t**)app_pool->apps->elts;
	
	for(i = 0;i<app_pool->apps->nelts;i++){
	
		app = apps[i];

		app_session = app->app_session_create(app,pkt_udp);
		if(app_session){
		
			app_session->app = app;

			return app_session;
		
		}
	}


	return NULL;
}

int  ch_udp_app_session_request_process(ch_udp_session_request_t *req_session ch_unused,ch_udp_app_session_t *app_session,
	ch_packet_udp_t *pkt_udp){

	return app_session->app->process_request_session(app_session,pkt_udp);
}

int  ch_udp_app_session_request_equal(ch_udp_session_request_t *req_session ch_unused,ch_udp_app_session_t *app_session,
	ch_packet_udp_t *pkt_udp){

	return app_session->app->request_session_equal(app_session,pkt_udp);
}

int  ch_udp_app_session_packet_process(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){

	ch_udp_app_t *app = app_session->app;

	if(app->is_request(app_session,pkt_udp))
		return app->req_pkt_process(app_session,pkt_udp);
	else
		return app->res_pkt_process(app_session,pkt_udp);

}

ssize_t  
ch_udp_app_session_write(ch_udp_session_t *udp_session,ch_udp_app_session_t *app_session,ch_data_output_t *dout)
{

	ssize_t len = 0,rc;
	/*write udp session */
	CH_DOUT_UINT64_WRITE(dout,udp_session->session_id,len,rc);
	CH_DOUT_UINT32_WRITE(dout,ch_udp_session_srcip_get(udp_session),len,rc);
	CH_DOUT_UINT32_WRITE(dout,ch_udp_session_dstip_get(udp_session),len,rc);
	CH_DOUT_UINT16_WRITE(dout,ch_udp_session_srcport_get(udp_session),len,rc);
	CH_DOUT_UINT16_WRITE(dout,ch_udp_session_dstport_get(udp_session),len,rc);
	
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_req_packets(udp_session),len,rc);
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_req_bytes(udp_session),len,rc);
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_res_packets(udp_session),len,rc);
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_res_bytes(udp_session),len,rc);
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_req_start_time(udp_session),len,rc);
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_req_last_time(udp_session),len,rc);
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_res_start_time(udp_session),len,rc);
	CH_DOUT_UINT64_WRITE(dout,ch_udp_session_res_last_time(udp_session),len,rc);

	/*write app session*/
	rc = app_session->app->app_session_write(app_session,dout);
	if(rc == -1)
		return -1;


	return len+rc;
}

int  
ch_udp_app_session_store(ch_udp_session_t *udp_session,ch_udp_app_session_t *app_session,ch_msgpack_store_t *dstore){

    ch_msgpack_store_map_start(dstore,NULL,2);

    ch_msgpack_store_map_start(dstore,"common",13);

	ch_msgpack_store_write_uint64(dstore,"sessionID",udp_session->session_id);
	ch_msgpack_store_write_uint32(dstore,"srcIP",ch_udp_session_srcip_get(udp_session));
	ch_msgpack_store_write_uint32(dstore,"dstIP",ch_udp_session_dstip_get(udp_session));
	ch_msgpack_store_write_uint16(dstore,"srcPort",ch_udp_session_srcport_get(udp_session));
	ch_msgpack_store_write_uint16(dstore,"dstPort",ch_udp_session_dstport_get(udp_session));
	
	ch_msgpack_store_write_uint64(dstore,"reqPackets",ch_udp_session_req_packets(udp_session));
	ch_msgpack_store_write_uint64(dstore,"reqBytes",ch_udp_session_req_bytes(udp_session));
	ch_msgpack_store_write_uint64(dstore,"resPackets",ch_udp_session_res_packets(udp_session));
	ch_msgpack_store_write_uint64(dstore,"resBytes",ch_udp_session_res_bytes(udp_session));
	ch_msgpack_store_write_uint64(dstore,"reqStartTime",ch_udp_session_req_start_time(udp_session));
	ch_msgpack_store_write_uint64(dstore,"reqLastTime",ch_udp_session_req_last_time(udp_session));
	ch_msgpack_store_write_uint64(dstore,"resStartTime",ch_udp_session_res_start_time(udp_session));
	ch_msgpack_store_write_uint64(dstore,"resLastTime",ch_udp_session_res_last_time(udp_session));

    if(app_session->app->app_session_store(app_session,dstore)<0)
        return -1;

    return 0;
}

void ch_udp_app_session_dump(ch_udp_app_session_t *app_session,ch_udp_session_t *udp_session,FILE *fp){
	
	char b[64] = {0};

	fprintf(fp,"Dump UDP APP Informations---------------------------------------\n");
	fprintf(fp,"udp.session.id:%lu\n",(unsigned long)udp_session->session_id);
	ch_ip_to_str(b,64,ch_udp_session_srcip_get(udp_session));
	fprintf(fp,"udp.session.sip:%s\n",b);
	ch_ip_to_str(b,64,ch_udp_session_dstip_get(udp_session));
	fprintf(fp,"udp.session.dip:%s\n",b);
	fprintf(fp,"udp.session.sport:%lu\n",(unsigned long)ch_udp_session_srcport_get(udp_session));
	fprintf(fp,"udp.session.dport:%lu\n",(unsigned long)ch_udp_session_dstport_get(udp_session));
	fprintf(fp,"udp.session.req.packets:%lu\n",(unsigned long)ch_udp_session_req_packets(udp_session));
	fprintf(fp,"udp.session.req.bytes:%lu\n",(unsigned long)ch_udp_session_req_bytes(udp_session));
	fprintf(fp,"udp.session.res.packets:%lu\n",(unsigned long)ch_udp_session_res_packets(udp_session));
	fprintf(fp,"udp.session.res.bytes:%lu\n",(unsigned long)ch_udp_session_res_bytes(udp_session));
	fprintf(fp,"udp.session.req.startTime:%lu\n",(unsigned long)ch_udp_session_req_start_time(udp_session));
	fprintf(fp,"udp.session.req.lastTime:%lu\n",(unsigned long)ch_udp_session_req_last_time(udp_session));
	fprintf(fp,"udp.session.res.startTime:%lu\n",(unsigned long)ch_udp_session_res_start_time(udp_session));
	fprintf(fp,"udp.session.res.lastTime:%lu\n",(unsigned long)ch_udp_session_res_last_time(udp_session));

	if(app_session->app->app_session_dump)
		app_session->app->app_session_dump(app_session,fp);

}


void ch_udp_app_session_fin(ch_udp_app_session_t *app_session){
	
	if(app_session->app->app_session_fin)
		app_session->app->app_session_fin(app_session);

}
