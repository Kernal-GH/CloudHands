/*
 *
 *      Filename: ch_dns_rdata_ipv4.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-27 14:59:38
 * Last Modified: 2018-05-09 15:53:07
 */

#include "ch_dns_rdata_ipv4.h"
#include "ch_net_util.h"

static void _rdata_ipv4_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	char b[64] = {0};

	ch_dns_rdata_ipv4_t *ipv4 = (ch_dns_rdata_ipv4_t*)rdata;

	ch_ip_to_str(b,64,ipv4->address);

	fprintf(fp,"Dump The rdata.ipv4:\n");
	fprintf(fp,"rdata.ipv4.address:%s\n",b);

}

static void _rdata_ipv4_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore) {

	ch_dns_rdata_ipv4_t *ipv4 = (ch_dns_rdata_ipv4_t*)rdata;
    ch_msgpack_store_map_start(dstore,"ipv4",1);
    ch_msgpack_store_write_uint32(dstore,"addr",ipv4->address);

}

static ssize_t  _rdata_ipv4_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_ipv4_t *ipv4 = (ch_dns_rdata_ipv4_t*)rdata;

	return ch_dout_uint32_write(dout,ipv4->address);
}

static ch_dns_rdata_t * _rdata_ipv4_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_ipv4_t *rdata = (ch_dns_rdata_ipv4_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_ipv4_t));

	rdata->address = 0;
	rdata->rdata.rdata_dump = _rdata_ipv4_dump;
	rdata->rdata.rdata_write = _rdata_ipv4_write;
    rdata->rdata.rdata_store = _rdata_ipv4_store;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_ipv4_parse(ch_pool_t *mp ch_unused,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_ipv4_t *rdata_ipv4 = (ch_dns_rdata_ipv4_t*)rdata;
	ch_dns_data_input_t din;

	if(rdata->dlen<4 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(&din,rdata);

	rdata_ipv4->address = ch_dns_data_input_uint32_read(&din);

	return 0;
}

static ch_dns_rdata_parser_t ipv4_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_a,
	_rdata_ipv4_create,
	_rdata_ipv4_parse
};


void ch_dns_rdata_ipv4_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&ipv4_parser);

}
