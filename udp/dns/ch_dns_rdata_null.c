/*
 *
 *      Filename: ch_dns_rdata_null.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 18:55:14
 * Last Modified: 2018-05-11 11:54:19
 */

#include "ch_dns_rdata_null.h"

static ch_dns_rdata_t * _rdata_null_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_null_t *rdata = (ch_dns_rdata_null_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_null_t));

	rdata->rdata.rdata_dump = NULL;
	rdata->rdata.rdata_write = NULL;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_null_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	void *addr = ch_pcalloc(mp,rdata->dlen+32);
	memcpy(addr,rdata->data,rdata->dlen);
	rdata->data = addr;
	return 0;
}


static ch_dns_rdata_parser_t null_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_null,
	_rdata_null_create,
	_rdata_null_parse
};


void ch_dns_rdata_null_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&null_parser);

}


