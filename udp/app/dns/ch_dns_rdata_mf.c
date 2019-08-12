/*
 *
 *      Filename: ch_dns_rdata_mf.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-05-09 16:35:34
 */

#include "ch_dns_rdata_mf.h"
#include "ch_log.h"

static void _rdata_mf_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_mf_t *mf = (ch_dns_rdata_mf_t*)rdata;

	fprintf(fp,"Dump The rdata.mf:\n");

	ch_dns_name_dump(&mf->name,fp);

}

static ssize_t  _rdata_mf_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_mf_t *mf = (ch_dns_rdata_mf_t*)rdata;

	return ch_dns_name_write(dout,&mf->name);

}

static void _rdata_mf_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_mf_t *mf = (ch_dns_rdata_mf_t*)rdata; 
    ch_msgpack_store_map_start(dstore,"mf",1);
    ch_dns_name_store(&mf->name,dstore);
}

static ch_dns_rdata_t * _rdata_mf_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_mf_t *rdata = (ch_dns_rdata_mf_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_mf_t));

	ch_dns_name_t *name = &rdata->name;

	rdata->rdata.rdata_dump = _rdata_mf_dump;
	rdata->rdata.rdata_write = _rdata_mf_write;
    rdata->rdata.rdata_store = _rdata_mf_store;

	CH_DNS_NAME_INIT(name);


	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_mf_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_mf_t *rdata_mf = (ch_dns_rdata_mf_t*)rdata;
	
	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	if(ch_dns_rdata_name_parse(mp,rdata,&rdata_mf->name)){
	
		ch_log(CH_LOG_ERR,"Parse MailBox failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t mf_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_mf,
	_rdata_mf_create,
	_rdata_mf_parse
};


void ch_dns_rdata_mf_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&mf_parser);

}

