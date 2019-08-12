/*
 *
 *      Filename: ch_dns_rdata_key.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-13 16:49:25
 */


#include "ch_dns_rdata_key.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_key_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_key_t *key = (ch_dns_rdata_key_t*)rdata;

	fprintf(fp,"Dump The rdata.key:\n");

	ch_dns_sec_key_dump(&key->skey,fp);

}

static ssize_t  _rdata_key_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_key_t *key = (ch_dns_rdata_key_t*)rdata;
	ssize_t rc,len = 0;

	ch_dns_sec_key_t *skey = &key->skey;

	CH_DNS_SEC_KEY_WRITE(dout,skey,len,rc);


	return len;
}

static void _rdata_key_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_key_t *key = (ch_dns_rdata_key_t*)rdata; 
     ch_msgpack_store_map_start(dstore,"key",1);
     ch_dns_sec_key_store(&key->skey,dstore);

}

static ch_dns_rdata_t * _rdata_key_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_key_t *key = (ch_dns_rdata_key_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_key_t));

	key->rdata.rdata_dump = _rdata_key_dump;
	key->rdata.rdata_write = _rdata_key_write;
	key->rdata.rdata_store = _rdata_key_store;

	ch_dns_sec_key_init(&key->skey);

	return (ch_dns_rdata_t*)key;

}

static int _rdata_key_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_key_t *key = (ch_dns_rdata_key_t*)rdata;
	
	if(rdata->dlen <4 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	ch_dns_sec_key_parse(mp,&key->skey,din);

	return 0;
}


static ch_dns_rdata_parser_t key_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_key,
	_rdata_key_create,
	_rdata_key_parse
};


void ch_dns_rdata_key_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&key_parser);
}


