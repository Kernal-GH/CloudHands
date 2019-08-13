/*
 *
 *      Filename: ch_dns_rdata_tkey.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 14:01:51
 */


#include "ch_dns_rdata_tkey.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_tkey_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_tkey_t *tkey = (ch_dns_rdata_tkey_t*)rdata;


	fprintf(fp,"Dump The rdata.tkey:\n");
	
	fprintf(fp,"rdata.tkey.alg:\n");
	ch_dns_name_dump(&tkey->alg,fp);

	fprintf(fp,"rdata.tkey.time_inception:%lu\n",(unsigned long)tkey->time_inception);
	fprintf(fp,"rdata.tkey.time_expire:%lu\n",(unsigned long)tkey->time_expire);

	fprintf(fp,"rdata.tkey.mode:%u\n",tkey->mode);
	fprintf(fp,"rdata.tkey.error:%u\n",tkey->error);

	ch_base64_string_dump("rdata.tkey.key",tkey->key,tkey->key_len,fp);
	ch_base64_string_dump("rdata.tkey.other",tkey->other,tkey->other_len,fp);

}

static ssize_t  _rdata_tkey_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_tkey_t *tkey = (ch_dns_rdata_tkey_t*)rdata;
	ssize_t rc,len = 0;

	CH_DNS_NAME_WRITE(dout,&tkey->alg,len,rc);

	CH_DOUT_UINT32_WRITE(dout,tkey->time_inception,len,rc);
	CH_DOUT_UINT32_WRITE(dout,tkey->time_expire,len,rc);
	CH_DOUT_UINT16_WRITE(dout,tkey->mode,len,rc);
	CH_DOUT_UINT16_WRITE(dout,tkey->error,len,rc);

	CH_DOUT_STRING16_WRITE(dout,tkey->key,tkey->key_len,len,rc);
	CH_DOUT_STRING16_WRITE(dout,tkey->other,tkey->other_len,len,rc);

	return len;
}

static void _rdata_tkey_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_tkey_t *tkey = (ch_dns_rdata_tkey_t*)rdata;
    ch_msgpack_store_map_start(dstore,"tkey",7);
    ch_dns_name_store(&tkey->alg,dstore);
    ch_msgpack_store_write_uint32(dstore,"timeInception",tkey->time_inception);
    ch_msgpack_store_write_uint32(dstore,"timeExpire",tkey->time_expire);
    ch_msgpack_store_write_uint16(dstore,"mode",tkey->mode);
    ch_msgpack_store_write_uint16(dstore,"error",tkey->error);
    ch_msgpack_store_write_bin_kv(dstore,"key",(void*)tkey->key,tkey->key_len);
    ch_msgpack_store_write_bin_kv(dstore,"other",(void*)tkey->other,tkey->other_len);

}

static ch_dns_rdata_t * _rdata_tkey_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_tkey_t *tkey = (ch_dns_rdata_tkey_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_tkey_t));

	tkey->rdata.rdata_dump = _rdata_tkey_dump;
	tkey->rdata.rdata_write = _rdata_tkey_write;
	tkey->rdata.rdata_store = _rdata_tkey_store;

	CH_DNS_NAME_INIT(&tkey->alg);
	tkey->time_inception = 0;
	tkey->time_expire = 0;
	tkey->mode = 0;
	tkey->error = 0;
	tkey->key_len = 0;
	tkey->key = NULL;

	tkey->other_len = 0;
	tkey->other = NULL;

	return (ch_dns_rdata_t*)tkey;

}

static int _rdata_tkey_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_tkey_t *tkey = (ch_dns_rdata_tkey_t*)rdata;
	
	if(rdata->dlen <16 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	CH_DNS_NAME_PARSE(mp,din,&tkey->alg);

	tkey->time_inception = ch_dns_data_input_uint32_read(din);
	tkey->time_expire = ch_dns_data_input_uint32_read(din);
	tkey->mode = ch_dns_data_input_uint16_read(din);
	tkey->error = ch_dns_data_input_uint16_read(din);

	tkey->key_len = ch_dns_data_input_uint16_read(din);
	CH_DNS_DLEN_CHECK(tkey->key_len,din,-1);
	if(tkey->key_len){
	
		tkey->key = ch_dns_data_input_bytes_read(din,mp,tkey->key_len);
	}

	tkey->other_len = ch_dns_data_input_uint16_read(din);

	CH_DNS_DLEN_CHECK(tkey->other_len,din,-1);
	if(tkey->other_len){
	
		tkey->other = ch_dns_data_input_bytes_read(din,mp,tkey->other_len);
	}

	return 0;
}


static ch_dns_rdata_parser_t tkey_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_tkey,
	_rdata_tkey_create,
	_rdata_tkey_parse
};


void ch_dns_rdata_tkey_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&tkey_parser);
}


