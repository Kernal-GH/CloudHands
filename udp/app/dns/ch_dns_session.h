/*
 *
 *      Filename: ch_dns_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 09:44:55
 * Last Modified: 2018-05-08 09:44:55
 */

#ifndef CH_DNS_SESSION_H
#define CH_DNS_SESSION_H

typedef struct ch_dns_session_t ch_dns_session_t;

#include "ch_mpool.h"
#include "ch_dns_request.h"
#include "ch_dns_response.h"
#include "ch_data_output.h"
#include "ch_udp_app_pool.h"
#include "ch_dns_rdata_ipv4.h"
#include "ch_msgpack_store.h"

struct ch_dns_session_t {

	ch_udp_app_session_t app_session;

	ch_pool_t *mp;
	ch_dns_requst_t *dns_req;
	ch_dns_response_t *dns_res;
};

extern ch_dns_session_t * ch_dns_session_create(ch_pool_t *mp);

static inline void ch_dns_session_dump(ch_dns_session_t *ds,FILE *fp){


	fprintf(fp,"dns.session:\n");

	if(ds->dns_req == NULL){
	
		fprintf(fp,"dns.session.request:empty!");
	}else{
		ch_dns_request_dump(ds->dns_req,fp);
	}

	if(ds->dns_res == NULL){
	
		fprintf(fp,"dns.session.response:empty!");

	}else{
	
		ch_dns_response_dump(ds->dns_res,fp);
	}
}

static inline ssize_t ch_dns_session_write(ch_dns_session_t *ds,ch_data_output_t *dout){

	ssize_t rc,len = 0;
	if(ds->dns_req == NULL &&ds->dns_res == NULL)
		return 0;

	CH_DOUT_UINT8_WRITE(dout,ds->dns_req?1:0,len,rc);
	CH_DOUT_UINT8_WRITE(dout,ds->dns_res?1:0,len,rc);
	if(ds->dns_req){
	
		CH_DNS_REQUEST_WRITE(dout,ds->dns_req,len,rc);
	}
	if(ds->dns_res){
	
		CH_DNS_RESPONSE_WRITE(dout,ds->dns_res,len,rc);

	}

	return len;
}

static inline int ch_dns_session_store(ch_dns_session_t *ds,ch_msgpack_store_t *dstore){

	if(ds->dns_req == NULL &&ds->dns_res == NULL)
		return -1;

    int n = 3;
    if((ds->dns_req!=NULL)&&(ds->dns_res!=NULL))
        n = 4;

    ch_msgpack_store_map_start(dstore,"dns",n);
    ch_msgpack_store_write_uint8(dstore,"hasReq",ds->dns_req?1:0);
    ch_msgpack_store_write_uint8(dstore,"hasRes",ds->dns_res?1:0);
	
    if(ds->dns_req){
	
		ch_dns_request_store(dstore,ds->dns_req);
	}
	if(ds->dns_res){
	
		ch_dns_response_store(dstore,ds->dns_res);

	}

    return 0;
}

static inline const char *ch_dns_session_domain_get(ch_dns_session_t *ds){

    ch_dns_requst_t *dns_req = ds->dns_req;
    if(dns_req == NULL)
        return NULL;

    return ch_dns_request_name_get(dns_req);
}

static inline int ch_dns_session_ipv4s_walk(ch_dns_session_t *ds,void (*call_fun)(uint32_t address,void *user_data),void *user_data){

    int n = 0;
    ch_dns_rdata_t *rdata;
    ch_dns_rdata_ipv4_t *ipv4;

    ch_dns_response_t *dns_res = ds->dns_res;
    
    if(dns_res){

        list_for_each_entry(rdata,&dns_res->alist,node){
        
            if(rdata->type == dns_rdatatype_a){

                n++;
                ipv4 = (ch_dns_rdata_ipv4_t*)rdata;

                call_fun(ipv4->address,user_data);

            }
        }
    }

    return n;
}

#endif /*CH_DNS_SESSION_H*/
