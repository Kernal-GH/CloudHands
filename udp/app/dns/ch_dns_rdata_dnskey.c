/*
 *
 *      Filename: ch_dns_rdata_dnskey.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-13 11:19:51
 */


#include "ch_dns_rdata_dnskey.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_dnskey_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_dnskey_t *dnskey = (ch_dns_rdata_dnskey_t*)rdata;

	fprintf(fp,"Dump The rdata.dnskey:\n");

	ch_dns_sec_key_dump(&dnskey->skey,fp);

}

static ssize_t  _rdata_dnskey_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_dnskey_t *dnskey = (ch_dns_rdata_dnskey_t*)rdata;
	ssize_t rc,len = 0;

	ch_dns_sec_key_t *skey = &dnskey->skey;

	CH_DNS_SEC_KEY_WRITE(dout,skey,len,rc);


	return len;
}

static void _rdata_dnskey_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

	ch_dns_rdata_dnskey_t *dnskey = (ch_dns_rdata_dnskey_t*)rdata;
	ch_dns_sec_key_t *skey = &dnskey->skey;

    ch_msgpack_store_map_start(dstore,"dnskey",1);
    ch_dns_sec_key_store(skey,dstore);

}

static ch_dns_rdata_t * _rdata_dnskey_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_dnskey_t *dnskey = (ch_dns_rdata_dnskey_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_dnskey_t));

	dnskey->rdata.rdata_dump = _rdata_dnskey_dump;
	dnskey->rdata.rdata_write = _rdata_dnskey_write;
	dnskey->rdata.rdata_store = _rdata_dnskey_store;

	ch_dns_sec_key_init(&dnskey->skey);

	return (ch_dns_rdata_t*)dnskey;

}

static int _rdata_dnskey_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_dnskey_t *dnskey = (ch_dns_rdata_dnskey_t*)rdata;
	
	if(rdata->dlen <4 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	ch_dns_sec_key_parse(mp,&dnskey->skey,din);

	return 0;
}


static ch_dns_rdata_parser_t dnskey_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_dnskey,
	_rdata_dnskey_create,
	_rdata_dnskey_parse
};


void ch_dns_rdata_dnskey_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&dnskey_parser);
}


