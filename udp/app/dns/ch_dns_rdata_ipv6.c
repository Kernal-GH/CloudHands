/*
 *
 *      Filename: ch_dns_rdata_ipv6.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-27 14:59:38
 * Last Modified: 2018-05-09 18:26:34
 */

#include "ch_dns_rdata_ipv6.h"
#include "ch_net_util.h"

static void _rdata_ipv6_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	char b[64] = {0};

	ch_dns_rdata_ipv6_t *ipv6 = (ch_dns_rdata_ipv6_t*)rdata;


	fprintf(fp,"Dump The rdata.ipv6:\n");

	ch_ipv6_to_str(b,64,ipv6->address);

	fprintf(fp,"rdata.ipv6.address:%s\n",b);

}

static ssize_t  _rdata_ipv6_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_ipv6_t *ipv6 = (ch_dns_rdata_ipv6_t*)rdata;

	return ch_dout_string16_write(dout,ipv6->address,16);
}

static void _rdata_ipv6_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore) {

    ch_dns_rdata_ipv6_t *ipv6 = (ch_dns_rdata_ipv6_t*)rdata; 
    ch_msgpack_store_map_start(dstore,"ipv6",1);
    ch_msgpack_store_write_bin_kv(dstore,"addr",(void*)ipv6->address,16);
}

static ch_dns_rdata_t * _rdata_ipv6_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_ipv6_t *rdata = (ch_dns_rdata_ipv6_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_ipv6_t));

	rdata->address = NULL;

	rdata->rdata.rdata_dump = _rdata_ipv6_dump;
	rdata->rdata.rdata_write = _rdata_ipv6_write;
	rdata->rdata.rdata_store = _rdata_ipv6_store;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_ipv6_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_ipv6_t *rdata_ipv6 = (ch_dns_rdata_ipv6_t*)rdata;
	ch_dns_data_input_t din;

	if(rdata->dlen<16 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(&din,rdata);

	rdata_ipv6->address = ch_dns_data_input_bytes_read(&din,mp,16);

	return 0;
}

static ch_dns_rdata_parser_t ipv6_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_aaaa,
	_rdata_ipv6_create,
	_rdata_ipv6_parse
};


void ch_dns_rdata_ipv6_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&ipv6_parser);

}
