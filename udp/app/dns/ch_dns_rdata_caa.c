/*
 *
 *      Filename: ch_dns_rdata_caa.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-12 17:06:00
 */


#include "ch_dns_rdata_caa.h"
#include "ch_log.h"

static void _rdata_caa_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_caa_t *caa = (ch_dns_rdata_caa_t*)rdata;

	fprintf(fp,"Dump The rdata.caa:\n");
	fprintf(fp,"rdata.caa.flags:%u\n",caa->flags);
	fprintf(fp,"rdata.caa.tag:%s\n",caa->tag);
	fprintf(fp,"rdata.caa.value:%s\n",caa->value);
	
}

static ssize_t  _rdata_caa_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_caa_t *caa = (ch_dns_rdata_caa_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *tag = caa->tag;
	unsigned char *value = caa->value;

	CH_DOUT_UINT8_WRITE(dout,caa->flags,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,tag,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,value,len,rc);

	return len;
}

static void _rdata_caa_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_caa_t *caa = (ch_dns_rdata_caa_t*)rdata; 
    ch_msgpack_store_map_start(dstore,"caa",3);
    ch_msgpack_store_write_uint8(dstore,"flags",caa->flags);
    ch_msgpack_store_write_kv(dstore,"tag",(const char*)caa->tag);
    ch_msgpack_store_write_kv(dstore,"value",(const char*)caa->value);
}

static ch_dns_rdata_t * _rdata_caa_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_caa_t *caa = (ch_dns_rdata_caa_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_caa_t));

	caa->rdata.rdata_dump = _rdata_caa_dump;
	caa->rdata.rdata_write = _rdata_caa_write;
    caa->rdata.rdata_store = _rdata_caa_store;

	caa->flags = 0;
	caa->tag = NULL;
	caa->value = NULL;

	return (ch_dns_rdata_t*)caa;

}

static int _rdata_caa_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_caa_t *caa = (ch_dns_rdata_caa_t*)rdata;
	
	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);
	caa->flags = ch_dns_data_input_uint8_read(din);
	caa->tag = (unsigned char*)ch_dns_data_input_bstring_read_dup(din,mp);
	caa->value = ch_dns_data_input_rbytes_read(din,mp);


	return 0;
}

static ch_dns_rdata_parser_t caa_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_caa,
	_rdata_caa_create,
	_rdata_caa_parse
};


void ch_dns_rdata_caa_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&caa_parser);

}


