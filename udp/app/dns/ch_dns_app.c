/*
 *
 *      Filename: ch_dns_app.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 16:27:57
 * Last Modified: 2018-09-13 17:18:52
 */

#include "ch_dns_app.h"
#include "ch_dns_session.h"
#include "ch_mpool.h"
#include "ch_log.h"
#include "ch_packet_record.h"
#include "ch_dns_app_context.h"
#include "ch_net_util.h"

static ch_dns_rdata_pool_t *rdata_pool = NULL;

#define DNS_PORT_IS_MATCH(pkt_udp) ((pkt_udp)->dst_port == 53)

static ch_udp_app_session_t * _dns_app_session_create(ch_udp_app_t *app,ch_packet_udp_t *pkt_udp){

	ch_udp_app_session_t *app_session;
	ch_pool_t *mp;

	if(!DNS_PORT_IS_MATCH(pkt_udp))
		return NULL;

	mp = ch_pool_create(4096);

	if(mp == NULL){

		ch_log(CH_LOG_ERR,"Cannot create memory pool for dns session!");
		return NULL;
	
	}

	app_session =  (ch_udp_app_session_t*)ch_dns_session_create(mp);

	if(app_session){
	
		app_session->app = app;
		app_session->mp = mp;
		app_session->priv_data = NULL;
	}

	return app_session;

}

static int _dns_is_request(ch_udp_app_session_t *app_session ch_unused,ch_packet_udp_t *pkt_udp){

	return DNS_PORT_IS_MATCH(pkt_udp);
}

static int _dns_req_pkt_process(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){

	ch_dns_data_input_t tmp,*din= &tmp;
	void *base,*end;

	ch_dns_session_t *dns_s = (ch_dns_session_t*)app_session;
	
	if(dns_s->dns_req)
		return PROCESS_CONTINUE;

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

	dns_s->dns_req = ch_dns_request_parse(dns_s->mp,din);
	if(dns_s->dns_req == NULL)
		return PROCESS_ERR;

	return PROCESS_CONTINUE;

}

static int _dns_res_pkt_process(ch_udp_app_session_t *app_session,ch_packet_udp_t *pkt_udp){

	ch_dns_data_input_t tmp,*din= &tmp;
	void *base,*end;

	ch_dns_session_t *dns_s = (ch_dns_session_t*)app_session;
	
	if(dns_s->dns_res)
		return PROCESS_DONE;

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


	dns_s->dns_res = ch_dns_response_parse(dns_s->mp,rdata_pool,din);
	
	if(dns_s->dns_res == NULL)
		return PROCESS_ERR;
	
	return PROCESS_DONE;

}

static void add_ip_wblist(uint32_t address,void *user_data){

    char buf[64]={0};

    ch_dns_app_context_t *dns_context = (ch_dns_app_context_t*)user_data;

    ch_redis_ip_wblist_t *ip_wblist = dns_context->ip_wblist;

    ch_redis_ip_wblist_add(ip_wblist,address);

    ch_ip_to_str(buf,64,address);

    printf("add ip address:%s|%lu\n to ip wblist!\n",(const char*)buf,(unsigned long)address);

} 

static ssize_t _dns_session_write(ch_udp_app_session_t *app_session,ch_data_output_t *dout){

	ch_dns_session_t *dns_s = (ch_dns_session_t*)app_session;
    ch_dns_app_context_t *dns_context = (ch_dns_app_context_t*)app_session->app->context;
    const char *domain = NULL;

	if(dns_s == NULL)
		return 0;

    domain = ch_dns_session_domain_get(dns_s);

    if(domain!=NULL&&strlen(domain)>0){

        if(!ch_dns_app_context_wblist_domain_is_accept(dns_context,domain)){

            printf("Pass this domain:%s\n",domain);
            /*add ip to wblist*/
            
            ch_dns_session_ipv4s_walk(dns_s,add_ip_wblist,(void*)dns_context);

            return 0;
        }
    }

	return ch_dns_session_write(dns_s,dout);

}

static int _dns_session_store(ch_udp_app_session_t *app_session,ch_msgpack_store_t *dstore){

	ch_dns_session_t *dns_s = (ch_dns_session_t*)app_session;
    ch_dns_app_context_t *dns_context = (ch_dns_app_context_t*)app_session->app->context;
    const char *domain = NULL;

	if(dns_s == NULL)
		return -1;

    if(dns_s->dns_req == NULL &&dns_s->dns_res == NULL)
        return -1;

    domain = ch_dns_session_domain_get(dns_s);

    if(domain!=NULL&&strlen(domain)>0){

        if(!ch_dns_app_context_wblist_domain_is_accept(dns_context,domain)){

            printf("Pass this domain:%s\n",domain);
            /*add ip to wblist*/
            
            ch_dns_session_ipv4s_walk(dns_s,add_ip_wblist,(void*)dns_context);

            return -2;
        }
    }

	ch_dns_session_store(dns_s,dstore);
    
    return 0;
}

static void _dns_session_dump(ch_udp_app_session_t *app_session,FILE *fp){

	ch_dns_session_t *dns_s = (ch_dns_session_t*)app_session;
	
	if(dns_s == NULL)
		return;

	return ch_dns_session_dump(dns_s,fp);

}

static void _dns_session_fin(ch_udp_app_session_t *app_session){

	ch_dns_session_t *dns_s = (ch_dns_session_t*)app_session;

	ch_pool_destroy(dns_s->mp);

}


static ch_udp_app_t dns_app = {
	.context = NULL,
	.type = PKT_RECORD_TYPE_UDP_DNS,
	.app_session_create = _dns_app_session_create,
	.process_request_session = NULL,
	.request_session_equal = NULL,
	.is_request = _dns_is_request,
	.req_pkt_process = _dns_req_pkt_process,
	.res_pkt_process = _dns_res_pkt_process,
	.app_session_write = _dns_session_write,
    .app_session_store = _dns_session_store,
	.app_session_dump = _dns_session_dump,
	.app_session_fin = _dns_session_fin
};

int ch_dns_app_init(ch_udp_app_pool_t *upool,const char *cfname) {

	rdata_pool = ch_dns_rdata_pool_create(upool->mp);

    dns_app.context = ch_dns_app_context_create(cfname);  

    if(dns_app.context == NULL){


        return -1;
    }

	ch_udp_app_register(upool,&dns_app);

	return 0;
}
