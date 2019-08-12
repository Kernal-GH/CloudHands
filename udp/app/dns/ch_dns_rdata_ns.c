/*
 *
 *      Filename: ch_dns_rdata_ns.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-05-09 17:25:24
 */

#include "ch_dns_rdata_ns.h"
#include "ch_log.h"

static void _rdata_ns_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_ns_t *ns = (ch_dns_rdata_ns_t*)rdata;

	fprintf(fp,"Dump The rdata.ns:\n");

	ch_dns_name_dump(&ns->name,fp);

}

static ssize_t  _rdata_ns_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_ns_t *ns = (ch_dns_rdata_ns_t*)rdata;

	return ch_dns_name_write(dout,&ns->name);

}

static void _rdata_ns_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_ns_t *ns = (ch_dns_rdata_ns_t*)rdata; 
    ch_msgpack_store_map_start(dstore,"ns",1);
    ch_dns_name_store(&ns->name,dstore);

}

static ch_dns_rdata_t * _rdata_ns_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_ns_t *rdata = (ch_dns_rdata_ns_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_ns_t));

	ch_dns_name_t *name = &rdata->name;

	CH_DNS_NAME_INIT(name);

	rdata->rdata.rdata_dump = _rdata_ns_dump;
	rdata->rdata.rdata_write = _rdata_ns_write;
	rdata->rdata.rdata_store = _rdata_ns_store;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_ns_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_ns_t *rdata_ns = (ch_dns_rdata_ns_t*)rdata;
	
	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	if(ch_dns_rdata_name_parse(mp,rdata,&rdata_ns->name)){
	
		ch_log(CH_LOG_ERR,"Parse ns failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t ns_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_ns,
	_rdata_ns_create,
	_rdata_ns_parse
};


void ch_dns_rdata_ns_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&ns_parser);

}

