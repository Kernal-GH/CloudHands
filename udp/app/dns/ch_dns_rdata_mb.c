/*
 *
 *      Filename: ch_dns_rdata_mb.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-06-14 14:36:14
 */

#include "ch_dns_rdata_mb.h"
#include "ch_log.h"

static void _rdata_mb_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_mb_t *mb = (ch_dns_rdata_mb_t*)rdata;

	fprintf(fp,"Dump The rdata.mb:\n");

	ch_dns_name_dump(&mb->name,fp);

}

static ssize_t  _rdata_mb_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_mb_t *mb = (ch_dns_rdata_mb_t*)rdata;

	return ch_dns_name_write(dout,&mb->name);

}

static void _rdata_mb_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_mb_t *mb = (ch_dns_rdata_mb_t*)rdata;
    ch_msgpack_store_map_start(dstore,"mb",1);
    ch_dns_name_store(&mb->name,dstore);

}

static ch_dns_rdata_t * _rdata_mb_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_mb_t *mb = (ch_dns_rdata_mb_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_mb_t));

	ch_dns_name_t *name = &mb->name;

	mb->rdata.rdata_dump = _rdata_mb_dump;
	mb->rdata.rdata_write = _rdata_mb_write;
	mb->rdata.rdata_store = _rdata_mb_store;

	CH_DNS_NAME_INIT(name);


	return (ch_dns_rdata_t*)mb;

}

static int _rdata_mb_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_mb_t *mb = (ch_dns_rdata_mb_t*)rdata;
	
	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	if(ch_dns_rdata_name_parse(mp,rdata,&mb->name)){
	
		ch_log(CH_LOG_ERR,"Parse MailBox failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t mb_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_mb,
	_rdata_mb_create,
	_rdata_mb_parse
};


void ch_dns_rdata_mb_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&mb_parser);

}

