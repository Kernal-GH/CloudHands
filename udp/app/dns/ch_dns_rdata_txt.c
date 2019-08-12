/*
 *
 *      Filename: ch_dns_rdata_txt.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:46:10
 * Last Modified: 2018-06-15 10:32:31
 */

#include "ch_dns_rdata_txt.h"
#include "ch_log.h"


static void _rdata_txt_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_txt_t *txt = (ch_dns_rdata_txt_t*)rdata;

	fprintf(fp,"Dump The rdata.txt:\n");

	ch_dns_txt_base_dump(&txt->txt_base,fp);

}

static ssize_t  _rdata_txt_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_txt_t *txt = (ch_dns_rdata_txt_t*)rdata;

	ch_dns_txt_base_t *txt_base = &txt->txt_base;

	CH_DNS_TXT_BASE_WRITE(dout,txt_base,len,rc);

	
	return len;
}

static void _rdata_txt_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_txt_t *txt = (ch_dns_rdata_txt_t*)rdata; 
    ch_msgpack_store_map_start(dstore,"txt",1);
    ch_dns_txt_base_store(&txt->txt_base,dstore);
}

static ch_dns_rdata_t * _rdata_txt_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_txt_t *txt = (ch_dns_rdata_txt_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_txt_t));


	txt->rdata.rdata_dump = _rdata_txt_dump;
	txt->rdata.rdata_write = _rdata_txt_write;
	txt->rdata.rdata_store = _rdata_txt_store;

	ch_dns_txt_base_init(mp,&txt->txt_base);

	return (ch_dns_rdata_t*)txt;

}

static int _rdata_txt_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_txt_t *txt = (ch_dns_rdata_txt_t*)rdata;
	
	if(rdata->dlen == 0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	return ch_dns_txt_base_parse(mp,&txt->txt_base,din);
}

static ch_dns_rdata_parser_t txt_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_txt,
	_rdata_txt_create,
	_rdata_txt_parse
};


void ch_dns_rdata_txt_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&txt_parser);

}


