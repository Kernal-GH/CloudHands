/*
 *
 *      Filename: ch_dns_app.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 16:27:57
 * Last Modified: 2018-06-26 12:14:59
 */

#include "ch_dns_app.h"
#include "ch_dns_session.h"
#include "ch_mpool.h"
#include "ch_log.h"
#include "ch_packet_record.h"

static ch_dns_rdata_pool_t *rdata_pool = NULL;

static int _dns_is_accept(ch_packet_udp_t *pkt_udp,void *priv_data ch_unused){

	return pkt_udp->src_port == 53||pkt_udp->dst_port == 53;
}

static void* _dns_session_create(void *priv_data ch_unused){

	ch_pool_t *mp;

	mp = ch_pool_create(4096);

	if(mp == NULL){

		ch_log(CH_LOG_ERR,"Cannot create memory pool for dns session!");
		return NULL;
	
	}

	return (void*)ch_dns_session_create(mp);
}

#define is_request(pkt_udp) ((pkt_udp)->dst_port == 53)

static int _dns_is_request(ch_packet_udp_t *pkt_udp,void *priv_data ch_unused){

	return is_request(pkt_udp);
}

static int _dns_pkt_process(void *dns_session,ch_packet_udp_t *pkt_udp,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din= &tmp;
	void *base,*end;

	ch_dns_session_t *dns_s = (ch_dns_session_t*)dns_session;

	if(pkt_udp->pdata == NULL || pkt_udp->payload_len == 0){
	
		return PROCESS_CONTINUE;
	}

	if(pkt_udp->payload_len>512){

		ch_log(CH_LOG_ERR,"too large dns packet:%d",pkt_udp->payload_len);
		return PROCESS_ERR;
	}

	base = pkt_udp->pdata;
	end = base+pkt_udp->payload_len;
	ch_dns_data_input_init(din,base,base,end);

	if(is_request(pkt_udp)){
		
		if(dns_s->dns_req)
			return PROCESS_CONTINUE;

		dns_s->dns_req = ch_dns_request_parse(dns_s->mp,din);
		if(dns_s->dns_req == NULL)
			return PROCESS_ERR;

		return PROCESS_CONTINUE;

	}else{
		
		if(dns_s->dns_res)
			return PROCESS_DONE;

		dns_s->dns_res = ch_dns_response_parse(dns_s->mp,rdata_pool,din);
		if(dns_s->dns_res == NULL)
			return PROCESS_ERR;
		return PROCESS_DONE;
	}

}

static ssize_t _dns_session_write(ch_data_output_t *dout,void *dns_session,void *priv_data ch_unused){

	ch_dns_session_t *dns_s = (ch_dns_session_t*)dns_session;
	
	if(dns_s == NULL)
		return 0;

	return ch_dns_session_write(dns_s,dout);

}

static void _dns_session_dump(FILE *fp,void *dns_session,void *priv_data ch_unused){

	ch_dns_session_t *dns_s = (ch_dns_session_t*)dns_session;
	
	if(dns_s == NULL)
		return;

	return ch_dns_session_dump(dns_s,fp);

}

static void _dns_session_fin(void *dns_session,void *priv_data ch_unused){

	ch_dns_session_t *dns_s = (ch_dns_session_t*)dns_session;

	ch_pool_destroy(dns_s->mp);


}

static ch_udp_app_t dns_app = {
	.type = PKT_RECORD_TYPE_UDP_DNS, 
	.is_accept = _dns_is_accept,
	.is_request = _dns_is_request,
	.app_session_create = _dns_session_create,
	.pkt_process = _dns_pkt_process,
	.app_session_write = _dns_session_write,
	.app_session_dump = _dns_session_dump,
	.app_session_fin = _dns_session_fin
};

void ch_dns_app_init(ch_udp_app_pool_t *upool) {

	rdata_pool = ch_dns_rdata_pool_create(upool->mp);

	ch_udp_app_register(upool,&dns_app);
}
