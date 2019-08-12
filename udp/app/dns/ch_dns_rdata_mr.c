/*
 *
 *      Filename: ch_dns_rdata_mr.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-05-09 16:56:01
 */

#include "ch_dns_rdata_mr.h"
#include "ch_log.h"

static void _rdata_mr_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_mr_t *mr = (ch_dns_rdata_mr_t*)rdata;

	fprintf(fp,"Dump The rdata.mr:\n");

	ch_dns_name_dump(&mr->name,fp);

}

static ssize_t  _rdata_mr_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_mr_t *mr = (ch_dns_rdata_mr_t*)rdata;

	return ch_dns_name_write(dout,&mr->name);

}

static void _rdata_mr_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_mr_t *mr = (ch_dns_rdata_mr_t*)rdata;    
     ch_msgpack_store_map_start(dstore,"mr",1);
     ch_dns_name_store(&mr->name,dstore);

}

static ch_dns_rdata_t * _rdata_mr_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_mr_t *rdata = (ch_dns_rdata_mr_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_mr_t));

	ch_dns_name_t *name = &rdata->name;

	rdata->rdata.rdata_dump = _rdata_mr_dump;
	rdata->rdata.rdata_write = _rdata_mr_write;
	rdata->rdata.rdata_store = _rdata_mr_store;

	CH_DNS_NAME_INIT(name);


	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_mr_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_mr_t *rdata_mr = (ch_dns_rdata_mr_t*)rdata;
	
	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	if(ch_dns_rdata_name_parse(mp,rdata,&rdata_mr->name)){
	
		ch_log(CH_LOG_ERR,"Parse MailBox failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t mr_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_mr,
	_rdata_mr_create,
	_rdata_mr_parse
};


void ch_dns_rdata_mr_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&mr_parser);

}

