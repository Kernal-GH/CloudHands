/*
 *
 *      Filename: ch_dns_rdata_openpgpkey.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:56:48
 */


#include "ch_dns_rdata_openpgpkey.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_openpgpkey_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_openpgpkey_t *openpgpkey = (ch_dns_rdata_openpgpkey_t*)rdata;

	fprintf(fp,"Dump The rdata.openpgpkey:\n");

	ch_base64_string_dump("rdata.openpgpkey.key",openpgpkey->key,openpgpkey->key_len,fp);

}

static ssize_t  _rdata_openpgpkey_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_openpgpkey_t *openpgpkey = (ch_dns_rdata_openpgpkey_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *key = openpgpkey->key;
	uint16_t key_len = openpgpkey->key_len;


	CH_DOUT_STRING16_WRITE(dout,key,key_len,len,rc);

	return len;
}

static void _rdata_openpgpkey_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_openpgpkey_t *openpgpkey = (ch_dns_rdata_openpgpkey_t*)rdata;
    ch_msgpack_store_map_start(dstore,"openpgpkey",1);
    ch_msgpack_store_write_bin_kv(dstore,"key",(void*) openpgpkey->key,openpgpkey->key_len);

}

static ch_dns_rdata_t * _rdata_openpgpkey_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_openpgpkey_t *openpgpkey = (ch_dns_rdata_openpgpkey_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_openpgpkey_t));

	openpgpkey->rdata.rdata_dump = _rdata_openpgpkey_dump;
	openpgpkey->rdata.rdata_write = _rdata_openpgpkey_write;
	openpgpkey->rdata.rdata_store = _rdata_openpgpkey_store;

	openpgpkey->key_len = 0;
	openpgpkey->key = NULL;

	return (ch_dns_rdata_t*)openpgpkey;

}

static int _rdata_openpgpkey_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_openpgpkey_t *openpgpkey = (ch_dns_rdata_openpgpkey_t*)rdata;
	
	if(rdata->dlen  == 0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	openpgpkey->key_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(openpgpkey->key_len,din,-1);

	openpgpkey->key = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t openpgpkey_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_openpgpkey,
	_rdata_openpgpkey_create,
	_rdata_openpgpkey_parse
};


void ch_dns_rdata_openpgpkey_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&openpgpkey_parser);
}


