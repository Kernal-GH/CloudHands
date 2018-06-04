/*
 *
 *      Filename: ch_dns_rdata_mx.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-05-09 18:27:07
 */

#include "ch_dns_rdata_mx.h"
#include "ch_log.h"

static void _rdata_mx_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_mx_t *mx = (ch_dns_rdata_mx_t*)rdata;

	fprintf(fp,"Dump The rdata.mx:\n");
	fprintf(fp,"rdata.mx.preference:%u",mx->preference);
	ch_dns_name_dump(&mx->mail_exchange,fp);

}

static ssize_t  _rdata_mx_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_mx_t *mx = (ch_dns_rdata_mx_t*)rdata;

	if(-1 == (rc = ch_dout_uint16_write(dout,mx->preference)))
		return -1;

	len+= rc;
	
	if(-1 == (rc = ch_dns_name_write(dout,&mx->mail_exchange)))
		return -1;

	len+= rc;

	return len;
}

static ch_dns_rdata_t * _rdata_mx_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_mx_t *rdata = (ch_dns_rdata_mx_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_mx_t));

	ch_dns_name_t *name = &rdata->mail_exchange;

	CH_DNS_NAME_INIT(name);

	rdata->preference = 0;

	rdata->rdata.rdata_dump = _rdata_mx_dump;
	rdata->rdata.rdata_write = _rdata_mx_write;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_mx_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t din;

	ch_dns_rdata_mx_t *rdata_mx = (ch_dns_rdata_mx_t*)rdata;
	
	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(&din,rdata);

	rdata_mx->preference = ch_dns_data_input_uint16_read(&din);

	if(ch_dns_name_parse(mp,&din,&rdata_mx->mail_exchange)){
	
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

