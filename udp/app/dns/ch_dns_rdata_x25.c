/*
 *
 *      Filename: ch_dns_rdata_x25.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 18:06:02
 * Last Modified: 2018-06-26 14:04:34
 */

#include "ch_dns_rdata_x25.h"
#include "ch_log.h"

static void _rdata_x25_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_x25_t *x25 = (ch_dns_rdata_x25_t*)rdata;

	fprintf(fp,"Dump The  rdata.x25:\n");
	fprintf(fp,"rdata.x25.address:%s\n",x25->address);

}

static ssize_t  _rdata_x25_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_x25_t *x25 = (ch_dns_rdata_x25_t*)rdata;

	CH_DOUT_STRING16_WRITE2(dout,x25->address,len,rc);
	
	return len;

}

static void _rdata_x25_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_x25_t *x25 = (ch_dns_rdata_x25_t*)rdata;
     ch_msgpack_store_map_start(dstore,"x25",1);
     ch_msgpack_store_write_kv(dstore,"address",(const char*)x25->address);
}

static ch_dns_rdata_t * _rdata_x25_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_x25_t *x25 = (ch_dns_rdata_x25_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_x25_t));

	x25->rdata.rdata_dump = _rdata_x25_dump;
	x25->rdata.rdata_write = _rdata_x25_write;
	x25->rdata.rdata_store = _rdata_x25_store;

	x25->address = NULL;

	return (ch_dns_rdata_t*)x25;

}

static int _rdata_x25_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_x25_t *x25 = (ch_dns_rdata_x25_t*)rdata;
	ch_dns_data_input_t tmp,*din = &tmp;

	if(rdata->dlen <1 || rdata->data == NULL)
		return -1;
	
	ch_dns_rdata_input_init(din,rdata);

	x25->address = ch_dns_data_input_bstring_read_dup(din,mp);


	return 0;
}

static ch_dns_rdata_parser_t x25_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_x25,
	_rdata_x25_create,
	_rdata_x25_parse
};


void ch_dns_rdata_x25_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&x25_parser);

}

