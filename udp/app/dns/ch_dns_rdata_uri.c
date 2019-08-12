/*
 *
 *      Filename: ch_dns_rdata_uri.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:28:04
 * Last Modified: 2018-06-26 14:04:00
 */

#include "ch_dns_rdata_uri.h"
#include "ch_log.h"

static void _rdata_uri_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_uri_t *uri = (ch_dns_rdata_uri_t*)rdata;

	fprintf(fp,"Dump The rdata.uri:\n");
	fprintf(fp,"rdata.uri.priority:%lu\n",(unsigned long)uri->priority);
	fprintf(fp,"rdata.uri.weight:%lu\n",(unsigned long)uri->weight);
	fprintf(fp,"rdata.uri.target:%s\n",uri->target?uri->target:(unsigned char*)"");

}

static ssize_t  _rdata_uri_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_uri_t *uri = (ch_dns_rdata_uri_t*)rdata;
	

	CH_DOUT_UINT16_WRITE(dout,uri->priority,len,rc);
	CH_DOUT_UINT16_WRITE(dout,uri->weight,len,rc);

	CH_DOUT_STRING16_WRITE(dout,uri->target,uri->target_len,len,rc);

	return len;

}

static void _rdata_uri_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_uri_t *uri = (ch_dns_rdata_uri_t*)rdata;    
    ch_msgpack_store_map_start(dstore,"uri",3);
    ch_msgpack_store_write_uint16(dstore,"priority",uri->priority);
    ch_msgpack_store_write_uint16(dstore,"weight",uri->weight);
    ch_msgpack_store_write_str_wlen(dstore,"target",(const char*)uri->target,uri->target_len);

}

static ch_dns_rdata_t * _rdata_uri_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_uri_t *uri = (ch_dns_rdata_uri_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_uri_t));

	uri->rdata.rdata_dump = _rdata_uri_dump;
	uri->rdata.rdata_write = _rdata_uri_write;
	uri->rdata.rdata_store = _rdata_uri_store;
	
	uri->priority = 0;
	uri->weight = 0;

	uri->target_len = 0;
	uri->target = NULL;

	return (ch_dns_rdata_t*)uri;

}

static int _rdata_uri_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din = &tmp;
	ch_dns_rdata_uri_t *uri = (ch_dns_rdata_uri_t*)rdata;
	
	if(rdata->dlen <4 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	
	uri->priority = ch_dns_data_input_uint16_read(din);
	uri->weight = ch_dns_data_input_uint16_read(din);
	
	uri->target_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(uri->target_len,din,-1);

	if(uri->target_len){
	
		uri->target = ch_dns_data_input_bytes_read(din,mp,uri->target_len);

	}

	return 0;
}

static ch_dns_rdata_parser_t uri_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_uri,
	_rdata_uri_create,
	_rdata_uri_parse
};


void ch_dns_rdata_uri_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&uri_parser);

}

