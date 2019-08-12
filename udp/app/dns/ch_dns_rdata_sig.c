/*
 *
 *      Filename: ch_dns_rdata_sig.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-14 19:35:01
 */


#include "ch_dns_rdata_sig.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_sig_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_sig_t *sig = (ch_dns_rdata_sig_t*)rdata;

	fprintf(fp,"Dump The rdata.sig:\n");

	ch_dns_sec_sig_dump(&sig->sig,fp);

}

static ssize_t  _rdata_sig_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_sig_t *sig = (ch_dns_rdata_sig_t*)rdata;
	ssize_t rc,len = 0;

	ch_dns_sec_sig_t *ssig = &sig->sig;

	CH_DNS_SEC_SIG_WRITE(dout,ssig,len,rc);


	return len;
}

static void _rdata_sig_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_sig_t *sig = (ch_dns_rdata_sig_t*)rdata;

     ch_msgpack_store_map_start(dstore,"sig",1);
     ch_dns_sec_sig_store(&sig->sig,dstore);
}

static ch_dns_rdata_t * _rdata_sig_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_sig_t *sig = (ch_dns_rdata_sig_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_sig_t));

	sig->rdata.rdata_dump = _rdata_sig_dump;
	sig->rdata.rdata_write = _rdata_sig_write;
	sig->rdata.rdata_store = _rdata_sig_store;

	ch_dns_sec_sig_init(&sig->sig);

	return (ch_dns_rdata_t*)sig;

}

static int _rdata_sig_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_sig_t *sig = (ch_dns_rdata_sig_t*)rdata;
	
	if(rdata->dlen <18 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	ch_dns_sec_sig_parse(mp,&sig->sig,din);

	return 0;
}


static ch_dns_rdata_parser_t sig_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_sig,
	_rdata_sig_create,
	_rdata_sig_parse
};


void ch_dns_rdata_sig_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&sig_parser);
}


