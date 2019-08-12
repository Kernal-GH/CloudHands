/*
 *
 *      Filename: ch_dns_rdata_nsap_ptr.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-06-13 18:37:23
 */

#include "ch_dns_rdata_nsap_ptr.h"
#include "ch_log.h"

static void _rdata_nsap_ptr_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_nsap_ptr_t *nsap_ptr = (ch_dns_rdata_nsap_ptr_t*)rdata;

	fprintf(fp,"Dump The rdata.nsap_ptr:\n");

	ch_dns_name_dump(&nsap_ptr->name,fp);

}

static ssize_t  _rdata_nsap_ptr_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_nsap_ptr_t *nsap_ptr = (ch_dns_rdata_nsap_ptr_t*)rdata;

	return ch_dns_name_write(dout,&nsap_ptr->name);

}

static void _rdata_nsap_ptr_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_nsap_ptr_t *nsap_ptr = (ch_dns_rdata_nsap_ptr_t*)rdata; 
    ch_msgpack_store_map_start(dstore,"nsapptr",1);
    ch_dns_name_store(&nsap_ptr->name,dstore);
}

static ch_dns_rdata_t * _rdata_nsap_ptr_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_nsap_ptr_t *nsap_ptr = (ch_dns_rdata_nsap_ptr_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_nsap_ptr_t));

	ch_dns_name_t *name = &nsap_ptr->name;

	nsap_ptr->rdata.rdata_dump = _rdata_nsap_ptr_dump;
	nsap_ptr->rdata.rdata_write = _rdata_nsap_ptr_write;
	nsap_ptr->rdata.rdata_store = _rdata_nsap_ptr_store;

	CH_DNS_NAME_INIT(name);


	return (ch_dns_rdata_t*)nsap_ptr;

}

static int _rdata_nsap_ptr_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_nsap_ptr_t *nsap_ptr = (ch_dns_rdata_nsap_ptr_t*)rdata;
	
	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	if(ch_dns_rdata_name_parse(mp,rdata,&nsap_ptr->name)){
	
		ch_log(CH_LOG_ERR,"Parse Rdata.nsap_ptr failed!");
		return -1;
	}

	return 0;

}

static ch_dns_rdata_parser_t nsap_ptr_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_nsap_ptr,
	_rdata_nsap_ptr_create,
	_rdata_nsap_ptr_parse
};


void ch_dns_rdata_nsap_ptr_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&nsap_ptr_parser);

}

