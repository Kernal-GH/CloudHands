/*
 *
 *      Filename: ch_dns_rdata_mx.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-06-13 17:57:04
 */

#include "ch_dns_rdata_mx.h"
#include "ch_log.h"

static void _rdata_mx_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_mx_t *mx = (ch_dns_rdata_mx_t*)rdata;

	fprintf(fp,"Dump The rdata.mx:\n");
	fprintf(fp,"rdata.mx.preference:%u\n",mx->preference);
	ch_dns_name_dump(&mx->mail_exchange,fp);

}

static ssize_t  _rdata_mx_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_mx_t *mx = (ch_dns_rdata_mx_t*)rdata;
	ch_dns_name_t *name = &mx->mail_exchange;

	CH_DOUT_UINT16_WRITE(dout,mx->preference,len,rc);
	CH_DNS_NAME_WRITE(dout,name,len,rc);

	return len;
}

static void _rdata_mx_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_mx_t *mx = (ch_dns_rdata_mx_t*)rdata; 
    ch_dns_name_t *name = &mx->mail_exchange; 

    ch_msgpack_store_map_start(dstore,"mx",2);
    ch_msgpack_store_write_uint16(dstore,"preference",mx->preference);
    ch_dns_name_store(name,dstore);
}

static ch_dns_rdata_t * _rdata_mx_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_mx_t *mx = (ch_dns_rdata_mx_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_mx_t));

	ch_dns_name_t *name = &mx->mail_exchange;



	mx->rdata.rdata_dump = _rdata_mx_dump;
	mx->rdata.rdata_write = _rdata_mx_write;
	mx->rdata.rdata_store = _rdata_mx_store;
	
	mx->preference = 0;
	CH_DNS_NAME_INIT(name);

	return (ch_dns_rdata_t*)mx;

}

static int _rdata_mx_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din = &tmp;

	ch_dns_rdata_mx_t *mx = (ch_dns_rdata_mx_t*)rdata;
	
	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	mx->preference = ch_dns_data_input_uint16_read(din);

	if(ch_dns_name_parse(mp,din,&mx->mail_exchange)){
	
		ch_log(CH_LOG_ERR,"Parse mx mail exchange failed!");
		return -1;
	}

	return 0;
}


static ch_dns_rdata_parser_t mx_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_mx,
	_rdata_mx_create,
	_rdata_mx_parse
};


void ch_dns_rdata_mx_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&mx_parser);

}

