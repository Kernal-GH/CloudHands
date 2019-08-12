/*
 *
 *      Filename: ch_dns_rdata_minfo.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 16:08:34
 * Last Modified: 2018-06-13 17:49:59
 */

#include "ch_dns_rdata_minfo.h"
#include "ch_log.h"

static void _rdata_minfo_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_minfo_t *minfo = (ch_dns_rdata_minfo_t*)rdata;

	fprintf(fp,"Dump The rdata.minfo:\n");

	ch_dns_name_dump(&minfo->responsibleAddress,fp);
	ch_dns_name_dump(&minfo->errorAddress,fp);

}

static ssize_t  _rdata_minfo_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_minfo_t *minfo = (ch_dns_rdata_minfo_t*)rdata;
	ch_dns_name_t *responsibleAddress = &minfo->responsibleAddress;
	ch_dns_name_t *errorAddress = &minfo->errorAddress;

	CH_DNS_NAME_WRITE(dout,responsibleAddress,len,rc);
	CH_DNS_NAME_WRITE(dout,errorAddress,len,rc);
	
	return len;
}

static void _rdata_minfo_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_minfo_t *minfo = (ch_dns_rdata_minfo_t*)rdata; 
     ch_dns_name_t *responsibleAddress = &minfo->responsibleAddress; 
     ch_dns_name_t *errorAddress = &minfo->errorAddress;     

     ch_msgpack_store_map_start(dstore,"minfo",2);
     ch_dns_name_store_wkey(responsibleAddress,dstore,"responsibleAddress");
     ch_dns_name_store_wkey(errorAddress,dstore,"errorAddress"); 
}

static ch_dns_rdata_t * _rdata_minfo_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_minfo_t *minfo = (ch_dns_rdata_minfo_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_minfo_t));

	ch_dns_name_t *ra_name = &minfo->responsibleAddress;
	ch_dns_name_t *err_name = &minfo->errorAddress;


	minfo->rdata.rdata_dump = _rdata_minfo_dump;
	minfo->rdata.rdata_write = _rdata_minfo_write;
	minfo->rdata.rdata_store = _rdata_minfo_store;
	
	CH_DNS_NAME_INIT(ra_name);
	CH_DNS_NAME_INIT(err_name);


	return (ch_dns_rdata_t*)minfo;

}

static int _rdata_minfo_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_minfo_t *minfo = (ch_dns_rdata_minfo_t*)rdata;

	if(rdata->dlen ==0 || rdata->data == NULL)
		return -1;

	ch_dns_data_input_t tmp,*din = &tmp;
	ch_dns_rdata_input_init(din,rdata);

	if(ch_dns_name_parse(mp,din,&minfo->responsibleAddress)){
	
		ch_log(CH_LOG_ERR,"Parse minfo responsibleAddress failed!");
		return -1;
	}
	
	if(ch_dns_name_parse(mp,din,&minfo->errorAddress)){
	
		ch_log(CH_LOG_ERR,"Parse minfo errorAddress failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t minfo_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_minfo,
	_rdata_minfo_create,
	_rdata_minfo_parse
};


void ch_dns_rdata_minfo_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&minfo_parser);

}


