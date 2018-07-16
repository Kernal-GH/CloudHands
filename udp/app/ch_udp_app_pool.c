/*
 *
 *      Filename: ch_udp_app_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 15:49:46
 * Last Modified: 2018-05-11 17:39:40
 */

#include "ch_udp_app_pool.h"
#include "ch_log.h"
#include "ch_net_util.h"
#include "ch_dns_app.h"

ch_udp_app_pool_t * ch_udp_app_pool_create(ch_pool_t *mp){

	ch_udp_app_pool_t *upool = ch_pcalloc(mp,sizeof(*upool));
	upool->mp = mp;

	upool->apps = ch_array_make(mp,16,sizeof(ch_udp_app_t*));

	ch_dns_app_init(upool);

	return upool;
}


void* ch_udp_app_session_create(ch_udp_app_t *uapp,void *priv_data){


	if(uapp->app_session_create == NULL)
		return NULL;

	return uapp->app_session_create(priv_data);  
}

int  ch_udp_app_packet_process(ch_udp_app_t *uapp,ch_udp_session_t *usession,ch_packet_udp_t *pkt_udp,void *priv_data){

	if(uapp->pkt_process == NULL)
		return PROCESS_DONE;

	return uapp->pkt_process(usession->app_session,pkt_udp,priv_data);
}

ssize_t  ch_udp_app_session_write(ch_udp_app_t *uapp,ch_data_output_t *dout,ch_udp_session_t *udp_session,void *priv_data){

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
	if(uapp->app_session_write)
		return uapp->app_session_write(dout,udp_session->app_session,priv_data);


	return 0;
}

void ch_udp_app_session_dump(ch_udp_app_t *uapp,FILE *fp,ch_udp_session_t *udp_session,void *priv_data){

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

	if(uapp->app_session_dump)
		uapp->app_session_dump(fp,udp_session->app_session,priv_data);
}

void ch_udp_app_session_fin(ch_udp_app_t *uapp,ch_udp_session_t *usession,void *priv_data){

	if(uapp->app_session_fin)
		uapp->app_session_fin(usession->app_session,priv_data);

}
