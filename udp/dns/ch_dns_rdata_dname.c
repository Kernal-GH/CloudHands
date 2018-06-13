/*
 *
 *      Filename: ch_dns_rdata_dname.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 17:11:51
 * Last Modified: 2018-06-13 09:57:37
 */

#include "ch_dns_rdata_dname.h"
#include "ch_log.h"

static void _rdata_dname_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_dname_t *dname = (ch_dns_rdata_dname_t*)rdata;

	fprintf(fp,"Dump The DName Rdata:\n");

	ch_dns_name_dump(&dname->name,fp);

}

static ssize_t  _rdata_dname_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_dname_t *dname = (ch_dns_rdata_dname_t*)rdata;

	return ch_dns_name_write(dout,&dname->name);

}

static ch_dns_rdata_t * _rdata_dname_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_dname_t *rdata = (ch_dns_rdata_dname_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_dname_t));

	ch_dns_name_t *name = &rdata->name;

	CH_DNS_NAME_INIT(name);

	rdata->rdata.rdata_dump = _rdata_dname_dump;
	rdata->rdata.rdata_write = _rdata_dname_write;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_dname_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_dname_t *rdata_dname = (ch_dns_rdata_dname_t*)rdata;
	ch_dns_data_input_t din;

	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(&din,rdata);

	if(ch_dns_name_parse(mp,&din,&rdata_dname->name)){
	
		ch_log(CH_LOG_ERR,"Parse DNS CName failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t dname_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_dname,
	_rdata_dname_create,
	_rdata_dname_parse
};


void ch_dns_rdata_dname_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&dname_parser);

}
