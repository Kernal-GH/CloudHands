/*
 *
 *      Filename: ch_dns_rdata_md.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-05-09 16:33:34
 */

#include "ch_dns_rdata_md.h"
#include "ch_log.h"

static void _rdata_md_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_md_t *md = (ch_dns_rdata_md_t*)rdata;

	fprintf(fp,"Dump The rdata.md:\n");

	ch_dns_name_dump(&md->name,fp);

}

static ssize_t  _rdata_md_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_md_t *md = (ch_dns_rdata_md_t*)rdata;

	return ch_dns_name_write(dout,&md->name);

}

static void _rdata_md_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_md_t *md = (ch_dns_rdata_md_t*)rdata; 
    ch_msgpack_store_map_start(dstore,"md",1);
    ch_dns_name_store(&md->name,dstore);
}

static ch_dns_rdata_t * _rdata_md_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_md_t *rdata = (ch_dns_rdata_md_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_md_t));

	ch_dns_name_t *name = &rdata->name;

	rdata->rdata.rdata_dump = _rdata_md_dump;
	rdata->rdata.rdata_write = _rdata_md_write;
    rdata->rdata.rdata_store =  _rdata_md_store;

	CH_DNS_NAME_INIT(name);


	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_md_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){


	ch_dns_rdata_md_t *rdata_md = (ch_dns_rdata_md_t*)rdata;
	
	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	if(ch_dns_rdata_name_parse(mp,rdata,&rdata_md->name)){
	
		ch_log(CH_LOG_ERR,"Parse Rdata.md failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t md_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_md,
	_rdata_md_create,
	_rdata_md_parse
};


void ch_dns_rdata_md_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&md_parser);

}

