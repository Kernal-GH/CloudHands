/*
 *
 *      Filename: ch_dns_rdata_srv.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:28:04
 * Last Modified: 2018-06-15 10:49:36
 */

#include "ch_dns_rdata_srv.h"
#include "ch_log.h"

static void _rdata_srv_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_srv_t *srv = (ch_dns_rdata_srv_t*)rdata;

	fprintf(fp,"Dump The rdata.srv:\n");
	fprintf(fp,"rdata.srv.priority:%lu\n",(unsigned long)srv->priority);
	fprintf(fp,"rdata.srv.weight:%lu\n",(unsigned long)srv->weight);
	fprintf(fp,"rdata.srv.port:%lu\n",(unsigned long)srv->port);


	fprintf(fp,"rdata.srv.target:\n");
	ch_dns_name_dump(&srv->target,fp);

}

static ssize_t  _rdata_srv_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_srv_t *srv = (ch_dns_rdata_srv_t*)rdata;
	ch_dns_name_t *target = &srv->target;
	

	CH_DOUT_UINT16_WRITE(dout,srv->priority,len,rc);
	CH_DOUT_UINT16_WRITE(dout,srv->weight,len,rc);
	CH_DOUT_UINT16_WRITE(dout,srv->port,len,rc);

	CH_DNS_NAME_WRITE(dout,target,len,rc);


	return len;

}

static void _rdata_srv_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_srv_t *srv = (ch_dns_rdata_srv_t*)rdata;
     ch_dns_name_t *target = &srv->target;         

     ch_msgpack_store_map_start(dstore,"srv",4);
     ch_msgpack_store_write_uint16(dstore,"priority",srv->priority);
     ch_msgpack_store_write_uint16(dstore,"weight",srv->weight);
     ch_msgpack_store_write_uint16(dstore,"port",srv->port);

     ch_dns_name_store(target,dstore);
}

static ch_dns_rdata_t * _rdata_srv_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_srv_t *srv = (ch_dns_rdata_srv_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_srv_t));

	srv->rdata.rdata_dump = _rdata_srv_dump;
	srv->rdata.rdata_write = _rdata_srv_write;
	srv->rdata.rdata_store = _rdata_srv_store;

	ch_dns_name_t *target = &srv->target;

	srv->priority = 0;
	srv->weight = 0;
	srv->port = 0;

	CH_DNS_NAME_INIT(target);


	return (ch_dns_rdata_t*)srv;

}

static int _rdata_srv_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din = &tmp;
	ch_dns_rdata_srv_t *srv = (ch_dns_rdata_srv_t*)rdata;
	
	if(rdata->dlen <6 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	
	srv->priority = ch_dns_data_input_uint16_read(din);
	srv->weight = ch_dns_data_input_uint16_read(din);
	srv->port = ch_dns_data_input_uint16_read(din);
	
	if(ch_dns_name_parse(mp,din,&srv->target)){
	
		ch_log(CH_LOG_ERR,"Parse rdata.srv.target failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t srv_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_srv,
	_rdata_srv_create,
	_rdata_srv_parse
};


void ch_dns_rdata_srv_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&srv_parser);

}

