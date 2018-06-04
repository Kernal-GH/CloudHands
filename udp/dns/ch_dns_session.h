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

struct ch_dns_session_t {

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

#endif /*CH_DNS_SESSION_H*/
