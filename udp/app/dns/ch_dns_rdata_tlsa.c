/*
 *
 *      Filename: ch_dns_rdata_tlsa.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 14:02:19
 */


#include "ch_dns_rdata_tlsa.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_tlsa_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_tlsa_t *tlsa = (ch_dns_rdata_tlsa_t*)rdata;

	fprintf(fp,"Dump The rdata.tlsa:\n");
	fprintf(fp,"rdata.tlsa.cert_usage:%u\n",tlsa->cert_usage);
	fprintf(fp,"rdata.tlsa.selector:%u\n",tlsa->selector);
	fprintf(fp,"rdata.tlsa.matching_type:%u\n",tlsa->matching_type);


	ch_hex_string_dump("rdata.tlsa.cert.data",tlsa->cert_data,tlsa->cert_dlen,fp);

}

static ssize_t  _rdata_tlsa_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_tlsa_t *tlsa = (ch_dns_rdata_tlsa_t*)rdata;
	ssize_t rc,len = 0;


	CH_DOUT_UINT8_WRITE(dout,tlsa->cert_usage,len,rc);
	CH_DOUT_UINT8_WRITE(dout,tlsa->selector,len,rc);
	CH_DOUT_UINT8_WRITE(dout,tlsa->matching_type,len,rc);

	CH_DOUT_STRING16_WRITE(dout,tlsa->cert_data,tlsa->cert_dlen,len,rc);

	return len;
}

static void _rdata_tlsa_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_tlsa_t *tlsa = (ch_dns_rdata_tlsa_t*)rdata;  

    ch_msgpack_store_map_start(dstore,"tlsa",4);
    ch_msgpack_store_write_uint8(dstore,"certUsage",tlsa->cert_usage);
    ch_msgpack_store_write_uint8(dstore,"selector",tlsa->selector);
    ch_msgpack_store_write_uint8(dstore,"matchingType",tlsa->matching_type);
    ch_msgpack_store_write_bin_kv(dstore,"data",(void*)tlsa->cert_data,tlsa->cert_dlen);

}

static ch_dns_rdata_t * _rdata_tlsa_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_tlsa_t *tlsa = (ch_dns_rdata_tlsa_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_tlsa_t));

	tlsa->rdata.rdata_dump = _rdata_tlsa_dump;
	tlsa->rdata.rdata_write = _rdata_tlsa_write;
	tlsa->rdata.rdata_store = _rdata_tlsa_store;

	tlsa->cert_usage = 0;
	tlsa->selector = 0;
	tlsa->matching_type = 0;
	tlsa->cert_dlen = 0;
	tlsa->cert_data = NULL;

	return (ch_dns_rdata_t*)tlsa;

}

static int _rdata_tlsa_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_tlsa_t *tlsa = (ch_dns_rdata_tlsa_t*)rdata;
	
	if(rdata->dlen <3 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	tlsa->cert_usage = ch_dns_data_input_uint8_read(din);
	tlsa->selector = ch_dns_data_input_uint8_read(din);
	tlsa->matching_type = ch_dns_data_input_uint8_read(din);
	

	tlsa->cert_dlen = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(tlsa->cert_dlen,din,-1);
	
	tlsa->cert_data = ch_dns_data_input_bytes_read(din,mp,tlsa->cert_dlen);

	return 0;
}


static ch_dns_rdata_parser_t tlsa_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_tlsa,
	_rdata_tlsa_create,
	_rdata_tlsa_parse
};


void ch_dns_rdata_tlsa_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&tlsa_parser);
}


