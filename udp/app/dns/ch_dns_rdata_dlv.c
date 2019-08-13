/*
 *
 *      Filename: ch_dns_rdata_dlv.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:47:56
 */


#include "ch_dns_rdata_dlv.h"
#include "ch_log.h"

static void _rdata_dlv_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_dlv_t *dlv = (ch_dns_rdata_dlv_t*)rdata;

	fprintf(fp,"Dump The rdata.dlv:\n");
	fprintf(fp,"rdata.dlv.footprint:%u\n",dlv->footprint);
	fprintf(fp,"rdata.dlv.alg:%u\n",dlv->alg);
	fprintf(fp,"rdata.dlv.digestid:%u\n",dlv->digestid);

}

static ssize_t  _rdata_dlv_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_dlv_t *dlv = (ch_dns_rdata_dlv_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *digest = dlv->digest;
	uint16_t digest_len = dlv->digest_len;

	CH_DOUT_UINT16_WRITE(dout,dlv->footprint,len,rc);
	CH_DOUT_UINT8_WRITE(dout,dlv->alg,len,rc);
	CH_DOUT_UINT8_WRITE(dout,dlv->digestid,len,rc);

	CH_DOUT_STRING16_WRITE(dout,digest,digest_len,len,rc);

	return len;
}

static void _rdata_dlv_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_dlv_t *dlv = (ch_dns_rdata_dlv_t*)rdata;

    ch_msgpack_store_map_start(dstore,"dlv",4);

    ch_msgpack_store_write_uint16(dstore,"footprint",dlv->footprint);
    ch_msgpack_store_write_uint8(dstore,"alg",dlv->alg);
    ch_msgpack_store_write_uint8(dstore,"digestid",dlv->digestid);
    ch_msgpack_store_write_bin_kv(dstore,"digest",(void*)dlv->digest,dlv->digest_len);

}

static ch_dns_rdata_t * _rdata_dlv_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_dlv_t *dlv = (ch_dns_rdata_dlv_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_dlv_t));

	dlv->rdata.rdata_dump = _rdata_dlv_dump;
	dlv->rdata.rdata_write = _rdata_dlv_write;
    dlv->rdata.rdata_store = _rdata_dlv_store;

	dlv->footprint = 0;
	dlv->alg = 0;
	dlv->digestid = 0;
	dlv->digest_len = 0;
	dlv->digest = NULL;

	return (ch_dns_rdata_t*)dlv;

}

static int _rdata_dlv_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_dlv_t *dlv = (ch_dns_rdata_dlv_t*)rdata;
	
	if(rdata->dlen <4 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	dlv->footprint = ch_dns_data_input_uint16_read(din);
	dlv->alg = ch_dns_data_input_uint8_read(din);
	dlv->digestid = ch_dns_data_input_uint8_read(din);

	dlv->digest_len = ch_dns_data_input_rdlen(din);

	CH_DNS_DLEN_CHECK(dlv->digest_len,din,-1);
	
	dlv->digest = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t dlv_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_dlv,
	_rdata_dlv_create,
	_rdata_dlv_parse
};


void ch_dns_rdata_dlv_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&dlv_parser);
}


