/*
 *
 *      Filename: ch_dns_rdata_soa.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:28:04
 * Last Modified: 2018-05-09 18:27:35
 */

#include "ch_dns_rdata_soa.h"
#include "ch_log.h"

#define WRITE_INT(dout,v,len) do {			\
	if( -1 == ch_dout_uint32_write(dout,v))	\
		return -1;							\
	len+=4;									\
}while(0)

#define WRITE_NAME(dout,n,len,rc) do {			\
	if(-1 == (rc = ch_dns_name_write(dout,n)))	\
		return -1;								\
	len+=rc;									\
}while(0)

static void _rdata_soa_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_soa_t *soa = (ch_dns_rdata_soa_t*)rdata;

	fprintf(fp,"Dump The rdata.soa:\n");
	fprintf(fp,"rdata.soa.serial:%lu\n",(unsigned long)soa->serial);
	fprintf(fp,"rdata.soa.refresh:%lu\n",(unsigned long)soa->refresh);
	fprintf(fp,"rdata.soa.retry:%lu\n",(unsigned long)soa->retry);
	fprintf(fp,"rdata.soa.expire:%lu\n",(unsigned long)soa->expire);
	fprintf(fp,"rdata.soa.minimum:%lu\n",(unsigned long)soa->minimum);

	ch_dns_name_dump(&soa->host,fp);
	ch_dns_name_dump(&soa->admin,fp);

}

static ssize_t  _rdata_soa_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_soa_t *soa = (ch_dns_rdata_soa_t*)rdata;
	ch_dns_name_t *host = &soa->host;
	ch_dns_name_t *admin = &soa->admin;

	WRITE_INT(dout,soa->serial,len);
	WRITE_INT(dout,soa->refresh,len);
	WRITE_INT(dout,soa->retry,len);
	WRITE_INT(dout,soa->expire,len);
	WRITE_INT(dout,soa->minimum,len);
	WRITE_NAME(dout,host,len,rc);
	WRITE_NAME(dout,admin,len,rc);

	return len;

}

static ch_dns_rdata_t * _rdata_soa_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_soa_t *rdata = (ch_dns_rdata_soa_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_soa_t));

	ch_dns_name_t *host = &rdata->host;
	ch_dns_name_t *admin = &rdata->admin;

	CH_DNS_NAME_INIT(host);
	CH_DNS_NAME_INIT(admin);

	rdata->serial = 0;
	rdata->refresh = 0;
	rdata->retry = 0;
	rdata->expire = 0;
	rdata->minimum = 0;

	rdata->rdata.rdata_dump = _rdata_soa_dump;
	rdata->rdata.rdata_write = _rdata_soa_write;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_soa_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t din;
	ch_dns_rdata_soa_t *rdata_soa = (ch_dns_rdata_soa_t*)rdata;
	
	if(rdata->dlen ==4*5 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(&din,rdata);

	if(ch_dns_name_parse(mp,&din,&rdata_soa->host)){
	
		ch_log(CH_LOG_ERR,"Parse rdata.soa.host failed!");
		return -1;
	}
	
	if(ch_dns_name_parse(mp,&din,&rdata_soa->admin)){
	
		ch_log(CH_LOG_ERR,"Parse rdata.soa.admin failed!");
		return -1;
	}

	rdata_soa->serial = ch_dns_data_input_uint32_read(&din);
	rdata_soa->refresh = ch_dns_data_input_uint32_read(&din);
	rdata_soa->retry = ch_dns_data_input_uint32_read(&din);
	rdata_soa->expire = ch_dns_data_input_uint32_read(&din);
	rdata_soa->minimum = ch_dns_data_input_uint32_read(&din);

	return 0;
}

static ch_dns_rdata_parser_t soa_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_soa,
	_rdata_soa_create,
	_rdata_soa_parse
};


void ch_dns_rdata_soa_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&soa_parser);

}

