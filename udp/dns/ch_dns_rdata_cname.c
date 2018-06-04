/*
 *
 *      Filename: ch_dns_rdata_cname.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 17:11:51
 * Last Modified: 2018-05-09 12:10:59
 */

#include "ch_dns_rdata_cname.h"
#include "ch_log.h"

static void _rdata_cname_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_cname_t *cname = (ch_dns_rdata_cname_t*)rdata;

	fprintf(fp,"Dump The CName Rdata:\n");

	ch_dns_name_dump(&cname->name,fp);

}

static ssize_t  _rdata_cname_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_cname_t *cname = (ch_dns_rdata_cname_t*)rdata;

	return ch_dns_name_write(dout,&cname->name);

}

static ch_dns_rdata_t * _rdata_cname_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_cname_t *rdata = (ch_dns_rdata_cname_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_cname_t));

	ch_dns_name_t *name = &rdata->name;

	CH_DNS_NAME_INIT(name);

	rdata->rdata.rdata_dump = _rdata_cname_dump;
	rdata->rdata.rdata_write = _rdata_cname_write;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_cname_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_cname_t *rdata_cname = (ch_dns_rdata_cname_t*)rdata;
	ch_dns_data_input_t din;

	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(&din,rdata);

	if(ch_dns_name_parse(mp,&din,&rdata_cname->name)){
	
		ch_log(CH_LOG_ERR,"Parse DNS CName failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t cname_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_cname,
	_rdata_cname_create,
	_rdata_cname_parse
};


void ch_dns_rdata_cname_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&cname_parser);

}
