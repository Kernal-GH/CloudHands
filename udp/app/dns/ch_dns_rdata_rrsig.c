/*
 *
 *      Filename: ch_dns_rdata_rrsig.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-14 19:18:12
 */


#include "ch_dns_rdata_rrsig.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_rrsig_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_rrsig_t *rrsig = (ch_dns_rdata_rrsig_t*)rdata;

	fprintf(fp,"Dump The rdata.rrsig:\n");

	ch_dns_sec_sig_dump(&rrsig->sig,fp);

}

static ssize_t  _rdata_rrsig_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_rrsig_t *rrsig = (ch_dns_rdata_rrsig_t*)rdata;
	ssize_t rc,len = 0;

	ch_dns_sec_sig_t *sig = &rrsig->sig;

	CH_DNS_SEC_SIG_WRITE(dout,sig,len,rc);


	return len;
}

static void _rdata_rrsig_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_rrsig_t *rrsig = (ch_dns_rdata_rrsig_t*)rdata;
     ch_msgpack_store_map_start(dstore,"rrsig",1);
     ch_dns_sec_sig_store(&rrsig->sig,dstore);
}

static ch_dns_rdata_t * _rdata_rrsig_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_rrsig_t *rrsig = (ch_dns_rdata_rrsig_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_rrsig_t));

	rrsig->rdata.rdata_dump = _rdata_rrsig_dump;
	rrsig->rdata.rdata_write = _rdata_rrsig_write;
	rrsig->rdata.rdata_store = _rdata_rrsig_store;

	ch_dns_sec_sig_init(&rrsig->sig);

	return (ch_dns_rdata_t*)rrsig;

}

static int _rdata_rrsig_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_rrsig_t *rrsig = (ch_dns_rdata_rrsig_t*)rdata;
	
	if(rdata->dlen <18 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	ch_dns_sec_sig_parse(mp,&rrsig->sig,din);

	return 0;
}


static ch_dns_rdata_parser_t rrsig_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_rrsig,
	_rdata_rrsig_create,
	_rdata_rrsig_parse
};


void ch_dns_rdata_rrsig_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&rrsig_parser);
}


