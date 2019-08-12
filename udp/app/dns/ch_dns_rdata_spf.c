/*
 *
 *      Filename: ch_dns_rdata_spf.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:46:10
 * Last Modified: 2018-06-15 10:38:25
 */

#include "ch_dns_rdata_spf.h"
#include "ch_log.h"


static void _rdata_spf_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_spf_t *spf = (ch_dns_rdata_spf_t*)rdata;

	fprintf(fp,"Dump The rdata.spf:\n");

	ch_dns_txt_base_dump(&spf->txt_base,fp);

}

static ssize_t  _rdata_spf_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_spf_t *spf = (ch_dns_rdata_spf_t*)rdata;

	ch_dns_txt_base_t *txt_base = &spf->txt_base;

	CH_DNS_TXT_BASE_WRITE(dout,txt_base,len,rc);

	
	return len;
}

static void _rdata_spf_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_spf_t *spf = (ch_dns_rdata_spf_t*)rdata;

    ch_msgpack_store_map_start(dstore,"spf",1);

    ch_dns_txt_base_store(&spf->txt_base,dstore);

}

static ch_dns_rdata_t * _rdata_spf_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_spf_t *spf = (ch_dns_rdata_spf_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_spf_t));


	spf->rdata.rdata_dump = _rdata_spf_dump;
	spf->rdata.rdata_write = _rdata_spf_write;
	spf->rdata.rdata_store = _rdata_spf_store;

	ch_dns_txt_base_init(mp,&spf->txt_base);

	return (ch_dns_rdata_t*)spf;

}

static int _rdata_spf_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_spf_t *spf = (ch_dns_rdata_spf_t*)rdata;
	
	if(rdata->dlen == 0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	return ch_dns_txt_base_parse(mp,&spf->txt_base,din);
}

static ch_dns_rdata_parser_t spf_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_spf,
	_rdata_spf_create,
	_rdata_spf_parse
};


void ch_dns_rdata_spf_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&spf_parser);

}


