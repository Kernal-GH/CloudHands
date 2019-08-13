/*
 *
 *      Filename: ch_dns_rdata_nsap.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:53:02
 */


#include "ch_dns_rdata_nsap.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_nsap_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_nsap_t *nsap = (ch_dns_rdata_nsap_t*)rdata;

	fprintf(fp,"Dump The rdata.nsap:\n");

	ch_hex_string_dump("rdata.nsap.addr",nsap->addr,nsap->addr_len,fp);

}

static ssize_t  _rdata_nsap_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_nsap_t *nsap = (ch_dns_rdata_nsap_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *addr = nsap->addr;
	uint16_t addr_len = nsap->addr_len;

	CH_DOUT_STRING16_WRITE(dout,addr,addr_len,len,rc);

	return len;
}

static void _rdata_nsap_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_nsap_t *nsap = (ch_dns_rdata_nsap_t*)rdata;

    ch_msgpack_store_map_start(dstore,"nsap",1);
    ch_msgpack_store_write_bin_kv(dstore,"addr",(void*)nsap->addr,nsap->addr_len);
}

static ch_dns_rdata_t * _rdata_nsap_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_nsap_t *nsap = (ch_dns_rdata_nsap_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_nsap_t));

	nsap->rdata.rdata_dump = _rdata_nsap_dump;
	nsap->rdata.rdata_write = _rdata_nsap_write;
	nsap->rdata.rdata_store = _rdata_nsap_store;

	nsap->addr_len = 0;
	nsap->addr = NULL;

	return (ch_dns_rdata_t*)nsap;

}

static int _rdata_nsap_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_nsap_t *nsap = (ch_dns_rdata_nsap_t*)rdata;
	
	if(rdata->dlen  ==0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	nsap->addr_len = ch_dns_data_input_rdlen(din);

	CH_DNS_DLEN_CHECK(nsap->addr_len,din,-1);
	
	nsap->addr = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t nsap_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_nsap,
	_rdata_nsap_create,
	_rdata_nsap_parse
};


void ch_dns_rdata_nsap_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&nsap_parser);
}


