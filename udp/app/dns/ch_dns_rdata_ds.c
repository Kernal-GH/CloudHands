/*
 *
 *      Filename: ch_dns_rdata_ds.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:48:48
 */


#include "ch_dns_rdata_ds.h"
#include "ch_log.h"

static void _rdata_ds_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_ds_t *ds = (ch_dns_rdata_ds_t*)rdata;

	fprintf(fp,"Dump The rdata.ds:\n");
	fprintf(fp,"rdata.ds.footprint:%u\n",ds->footprint);
	fprintf(fp,"rdata.ds.alg:%u\n",ds->alg);
	fprintf(fp,"rdata.ds.digestid:%u\n",ds->digestid);

}

static ssize_t  _rdata_ds_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_ds_t *ds = (ch_dns_rdata_ds_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *digest = ds->digest;
	uint16_t digest_len = ds->digest_len;

	CH_DOUT_UINT16_WRITE(dout,ds->footprint,len,rc);
	CH_DOUT_UINT8_WRITE(dout,ds->alg,len,rc);
	CH_DOUT_UINT8_WRITE(dout,ds->digestid,len,rc);

	CH_DOUT_STRING16_WRITE(dout,digest,digest_len,len,rc);

	return len;
}

static void _rdata_ds_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_ds_t *ds = (ch_dns_rdata_ds_t*)rdata;

    ch_msgpack_store_map_start(dstore,"ds",4);

    ch_msgpack_store_write_uint16(dstore,"footprint",ds->footprint);
    ch_msgpack_store_write_uint8(dstore,"alg",ds->alg);
    ch_msgpack_store_write_uint8(dstore,"digestid",ds->digestid);
    ch_msgpack_store_write_bin_kv(dstore,"digest",(void*)ds->digest,ds->digest_len);

}

static ch_dns_rdata_t * _rdata_ds_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_ds_t *ds = (ch_dns_rdata_ds_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_ds_t));

	ds->rdata.rdata_dump = _rdata_ds_dump;
	ds->rdata.rdata_write = _rdata_ds_write;
	ds->rdata.rdata_store = _rdata_ds_store;

	ds->footprint = 0;
	ds->alg = 0;
	ds->digestid = 0;
	ds->digest_len = 0;
	ds->digest = NULL;

	return (ch_dns_rdata_t*)ds;

}

static int _rdata_ds_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_ds_t *ds = (ch_dns_rdata_ds_t*)rdata;
	
	if(rdata->dlen <4 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	ds->footprint = ch_dns_data_input_uint16_read(din);
	ds->alg = ch_dns_data_input_uint8_read(din);
	ds->digestid = ch_dns_data_input_uint8_read(din);

	ds->digest_len = ch_dns_data_input_rdlen(din);
	
	CH_DNS_DLEN_CHECK(ds->digest_len,din,-1);

	ds->digest = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t ds_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_ds,
	_rdata_ds_create,
	_rdata_ds_parse
};


void ch_dns_rdata_ds_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&ds_parser);
}


