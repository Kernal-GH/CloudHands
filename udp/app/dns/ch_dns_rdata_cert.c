/*
 *
 *      Filename: ch_dns_rdata_cert.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:46:13
 */


#include "ch_dns_rdata_cert.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_cert_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_cert_t *cert = (ch_dns_rdata_cert_t*)rdata;

	fprintf(fp,"Dump The rdata.cert:\n");
	fprintf(fp,"rdata.cert.type:%u\n",cert->cert_type);
	fprintf(fp,"rdata.cert.keytag:%u\n",cert->key_tag);
	fprintf(fp,"rdata.cert.alg:%u\n",cert->alg);

	ch_base64_string_dump("rdata.cert.cert",cert->cert,cert->cert_len,fp);

}

static ssize_t  _rdata_cert_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_cert_t *cert = (ch_dns_rdata_cert_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *cert_v = cert->cert;
	uint16_t cert_len = cert->cert_len;

	CH_DOUT_UINT16_WRITE(dout,cert->cert_type,len,rc);
	CH_DOUT_UINT16_WRITE(dout,cert->key_tag,len,rc);
	CH_DOUT_UINT8_WRITE(dout,cert->alg,len,rc);

	CH_DOUT_STRING16_WRITE(dout,cert_v,cert_len,len,rc);

	return len;
}

static void _rdata_cert_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_cert_t *cert = (ch_dns_rdata_cert_t*)rdata;

    ch_msgpack_store_map_start(dstore,"cert",4);
    ch_msgpack_store_write_uint16(dstore,"type",cert->cert_type);
    ch_msgpack_store_write_uint16(dstore,"tag",cert->key_tag);
    ch_msgpack_store_write_uint8(dstore,"alg",cert->alg);
    ch_msgpack_store_write_bin_kv(dstore,"value",(void*)cert->cert,cert->cert_len);
}

static ch_dns_rdata_t * _rdata_cert_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_cert_t *cert = (ch_dns_rdata_cert_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_cert_t));

	cert->rdata.rdata_dump = _rdata_cert_dump;
	cert->rdata.rdata_write = _rdata_cert_write;
    cert->rdata.rdata_store = _rdata_cert_store;

	cert->cert_type = 0;
	cert->key_tag = 0;
	cert->alg = 0;
	cert->cert_len = 0;
	cert->cert = NULL;

	return (ch_dns_rdata_t*)cert;

}

static int _rdata_cert_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_cert_t *cert = (ch_dns_rdata_cert_t*)rdata;
	
	if(rdata->dlen <5 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	cert->cert_type = ch_dns_data_input_uint16_read(din);
	cert->key_tag = ch_dns_data_input_uint16_read(din);
	cert->alg = ch_dns_data_input_uint8_read(din);

	cert->cert_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(cert->cert_len,din,-1);

	cert->cert = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t cert_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_cert,
	_rdata_cert_create,
	_rdata_cert_parse
};


void ch_dns_rdata_cert_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&cert_parser);
}


