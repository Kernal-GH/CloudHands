/*
 *
 *      Filename: ch_dns_rdata_smimea.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:59:41
 */


#include "ch_dns_rdata_smimea.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_smimea_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_smimea_t *smimea = (ch_dns_rdata_smimea_t*)rdata;

	fprintf(fp,"Dump The rdata.smimea:\n");
	fprintf(fp,"rdata.smimea.cert_usage:%u\n",smimea->cert_usage);
	fprintf(fp,"rdata.smimea.selector:%u\n",smimea->selector);
	fprintf(fp,"rdata.smimea.matching_type:%u\n",smimea->matching_type);


	ch_hex_string_dump("rdata.smimea.cert.data",smimea->cert_data,smimea->cert_dlen,fp);

}

static ssize_t  _rdata_smimea_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_smimea_t *smimea = (ch_dns_rdata_smimea_t*)rdata;
	ssize_t rc,len = 0;


	CH_DOUT_UINT8_WRITE(dout,smimea->cert_usage,len,rc);
	CH_DOUT_UINT8_WRITE(dout,smimea->selector,len,rc);
	CH_DOUT_UINT8_WRITE(dout,smimea->matching_type,len,rc);

	CH_DOUT_STRING16_WRITE(dout,smimea->cert_data,smimea->cert_dlen,len,rc);

	return len;
}

static void _rdata_smimea_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_smimea_t *smimea = (ch_dns_rdata_smimea_t*)rdata;

    ch_msgpack_store_map_start(dstore,"smimea",4);
    ch_msgpack_store_write_uint8(dstore,"certUsage",smimea->cert_usage);
    ch_msgpack_store_write_uint8(dstore,"selector",smimea->selector);
    ch_msgpack_store_write_uint8(dstore,"matchingType",smimea->matching_type);

    ch_msgpack_store_write_bin_kv(dstore,"data",(void*)smimea->cert_data,smimea->cert_dlen);

}

static ch_dns_rdata_t * _rdata_smimea_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_smimea_t *smimea = (ch_dns_rdata_smimea_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_smimea_t));

	smimea->rdata.rdata_dump = _rdata_smimea_dump;
	smimea->rdata.rdata_write = _rdata_smimea_write;
	smimea->rdata.rdata_store = _rdata_smimea_store;

	smimea->cert_usage = 0;
	smimea->selector = 0;
	smimea->matching_type = 0;
	smimea->cert_dlen = 0;
	smimea->cert_data = NULL;

	return (ch_dns_rdata_t*)smimea;

}

static int _rdata_smimea_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_smimea_t *smimea = (ch_dns_rdata_smimea_t*)rdata;
	
	if(rdata->dlen <3 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	smimea->cert_usage = ch_dns_data_input_uint8_read(din);
	smimea->selector = ch_dns_data_input_uint8_read(din);
	smimea->matching_type = ch_dns_data_input_uint8_read(din);
	

	smimea->cert_dlen = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(smimea->cert_dlen,din,-1);
	
	smimea->cert_data = ch_dns_data_input_bytes_read(din,mp,smimea->cert_dlen);

	return 0;
}


static ch_dns_rdata_parser_t smimea_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_smimea,
	_rdata_smimea_create,
	_rdata_smimea_parse
};


void ch_dns_rdata_smimea_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&smimea_parser);
}


