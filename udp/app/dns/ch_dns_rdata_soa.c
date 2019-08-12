/*
 *
 *      Filename: ch_dns_rdata_soa.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:28:04
 * Last Modified: 2018-06-15 09:42:47
 */

#include "ch_dns_rdata_soa.h"
#include "ch_log.h"

static void _rdata_soa_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_soa_t *soa = (ch_dns_rdata_soa_t*)rdata;

	fprintf(fp,"Dump The rdata.soa:\n");
	fprintf(fp,"rdata.soa.serial:%lu\n",(unsigned long)soa->serial);
	fprintf(fp,"rdata.soa.refresh:%lu\n",(unsigned long)soa->refresh);
	fprintf(fp,"rdata.soa.retry:%lu\n",(unsigned long)soa->retry);
	fprintf(fp,"rdata.soa.expire:%lu\n",(unsigned long)soa->expire);
	fprintf(fp,"rdata.soa.minimum:%lu\n",(unsigned long)soa->minimum);

	fprintf(fp,"rdata.soa.host:\n");
	ch_dns_name_dump(&soa->host,fp);

	fprintf(fp,"rdata.soa.admin:\n");
	ch_dns_name_dump(&soa->admin,fp);

}

static ssize_t  _rdata_soa_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_soa_t *soa = (ch_dns_rdata_soa_t*)rdata;
	ch_dns_name_t *host = &soa->host;
	ch_dns_name_t *admin = &soa->admin;
	
	CH_DNS_NAME_WRITE(dout,host,len,rc);
	CH_DNS_NAME_WRITE(dout,admin,len,rc);

	CH_DOUT_UINT32_WRITE(dout,soa->serial,len,rc);
	CH_DOUT_UINT32_WRITE(dout,soa->refresh,len,rc);
	CH_DOUT_UINT32_WRITE(dout,soa->retry,len,rc);
	CH_DOUT_UINT32_WRITE(dout,soa->expire,len,rc);
	CH_DOUT_UINT32_WRITE(dout,soa->minimum,len,rc);



	return len;

}

static void _rdata_soa_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_soa_t *soa = (ch_dns_rdata_soa_t*)rdata;
    ch_dns_name_t *host = &soa->host;   
    ch_dns_name_t *admin = &soa->admin; 

    ch_msgpack_store_map_start(dstore,"soa",7);
    ch_dns_name_store_wkey(host,dstore,"host");
    ch_dns_name_store_wkey(admin,dstore,"admin");
    ch_msgpack_store_write_uint32(dstore,"serial",soa->serial);
    ch_msgpack_store_write_uint32(dstore,"refresh",soa->refresh);
    ch_msgpack_store_write_uint32(dstore,"retry",soa->retry);
    ch_msgpack_store_write_uint32(dstore,"expire",soa->expire);
    ch_msgpack_store_write_uint32(dstore,"minimum",soa->minimum);

}

static ch_dns_rdata_t * _rdata_soa_create(ch_pool_t *mp,void *priv_data ch_unused){



	ch_dns_rdata_soa_t *soa = (ch_dns_rdata_soa_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_soa_t));

	soa->rdata.rdata_dump = _rdata_soa_dump;
	soa->rdata.rdata_write = _rdata_soa_write;
	soa->rdata.rdata_store = _rdata_soa_store;
	
	ch_dns_name_t *host = &soa->host;
	ch_dns_name_t *admin = &soa->admin;

	CH_DNS_NAME_INIT(host);
	CH_DNS_NAME_INIT(admin);

	soa->serial = 0;
	soa->refresh = 0;
	soa->retry = 0;
	soa->expire = 0;
	soa->minimum = 0;



	return (ch_dns_rdata_t*)soa;

}

static int _rdata_soa_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din = &tmp;
	ch_dns_rdata_soa_t *soa = (ch_dns_rdata_soa_t*)rdata;
	
	if(rdata->dlen <20 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	if(ch_dns_name_parse(mp,din,&soa->host)){
	
		ch_log(CH_LOG_ERR,"Parse rdata.soa.host failed!");
		return -1;
	}
	
	if(ch_dns_name_parse(mp,din,&soa->admin)){
	
		ch_log(CH_LOG_ERR,"Parse rdata.soa.admin failed!");
		return -1;
	}

	soa->serial = ch_dns_data_input_uint32_read(din);
	soa->refresh = ch_dns_data_input_uint32_read(din);
	soa->retry = ch_dns_data_input_uint32_read(din);
	soa->expire = ch_dns_data_input_uint32_read(din);
	soa->minimum = ch_dns_data_input_uint32_read(din);

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

