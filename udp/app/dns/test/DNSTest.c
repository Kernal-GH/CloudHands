/*
 *
 *      Filename: DNSTest.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-11 17:53:39
 * Last Modified: 2018-05-11 19:04:45
 */

#include <stdio.h>
#include <stdlib.h>
#include "ch_dns_session.h"
#include "ch_mpool.h"
#include "ch_dns_rdata_pool.h"
#include "ch_dns_data_input.h"
#include "ch_log.h"

static ch_dns_rdata_pool_t *rdata_pool;

static void _parse_request(ch_dns_data_input_t *din,ch_pool_t *mp){

	ch_dns_requst_t *req = ch_dns_request_parse(mp,din);
	if(req == NULL)
		printf("Parse DNS Request Failed!\n");

	ch_dns_request_dump(req,stdout);

}

static void _parse_response(ch_dns_data_input_t *din,ch_pool_t *mp){

	ch_dns_response_t *res = ch_dns_response_parse(mp,rdata_pool,din);
	if(res == NULL)
		printf("Parse DNS Response Failed!\n");

	ch_dns_response_dump(res,stdout);

}

int main(int argc,char **argv){

	ch_dns_data_input_t tmp,*din= &tmp;

	char buf[4096] = {0};
	size_t rsize;
	FILE *fp;
	
	void *base = (void*)buf;
	int is_req = 0;

	ch_pool_t *mp;

	mp = ch_pool_create(4096);
    
	/*init log*/
    ch_log_init(mp,"DNSTest.log",CH_LOG_DEBUG);

	rdata_pool = ch_dns_rdata_pool_create(mp);

	fp = fopen(argv[1],"r");
	
	fread(buf,4095,1,fp);
	rsize = atoi(argv[2]);

	if(*argv[3]=='r')
		is_req = 1;

	ch_dns_data_input_init(din,base,base,base+rsize);

	if(is_req)
		_parse_request(din,mp);
	else
		_parse_response(din,mp);

	return 0;
}
