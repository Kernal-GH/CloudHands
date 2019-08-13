/*
 *
 *      Filename: ch_dns_rdata_null.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:55:50
 */


#include "ch_dns_rdata_null.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_null_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_null_t *null = (ch_dns_rdata_null_t*)rdata;

	fprintf(fp,"Dump The rdata.null:\n");
	ch_hex_string_dump("rdata.null.data",null->data,null->dlen,fp);

}

static ssize_t  _rdata_null_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_null_t *null = (ch_dns_rdata_null_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *data = null->data;
	uint16_t dlen = null->dlen;

	CH_DOUT_STRING16_WRITE(dout,data,dlen,len,rc);

	return len;
}

static void _rdata_null_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_null_t *null = (ch_dns_rdata_null_t*)rdata;

     ch_msgpack_store_map_start(dstore,"null",1);
     ch_msgpack_store_write_bin_kv(dstore,"data",(void*)null->data,null->dlen);

}

static ch_dns_rdata_t * _rdata_null_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_null_t *null = (ch_dns_rdata_null_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_null_t));

	null->rdata.rdata_dump = _rdata_null_dump;
	null->rdata.rdata_write = _rdata_null_write;
	null->rdata.rdata_store = _rdata_null_store;

	null->dlen = 0;
	null->data = NULL;

	return (ch_dns_rdata_t*)null;

}

static int _rdata_null_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_null_t *null = (ch_dns_rdata_null_t*)rdata;
	
	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	null->dlen = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(null->dlen,din,-1);

	null->data = ch_dns_data_input_rbytes_read(din,mp);

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


