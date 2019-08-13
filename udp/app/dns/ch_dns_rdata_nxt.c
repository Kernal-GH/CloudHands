/*
 *
 *      Filename: ch_dns_rdata_nxt.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:56:24
 */


#include "ch_dns_rdata_nxt.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_nxt_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_nxt_t *nxt = (ch_dns_rdata_nxt_t*)rdata;

	fprintf(fp,"Dump The rdata.nxt:\n");

	ch_dns_name_dump(&nxt->name,fp);

	ch_hex_string_dump("rdata.nxt.typebitmap",nxt->typebitmap,nxt->typebitmap_len,fp);

}

static ssize_t  _rdata_nxt_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_nxt_t *nxt = (ch_dns_rdata_nxt_t*)rdata;
	ssize_t rc,len = 0;

	ch_dns_name_t *name = &nxt->name;

	CH_DNS_NAME_WRITE(dout,name,len,rc);

	CH_DOUT_STRING16_WRITE(dout,nxt->typebitmap,nxt->typebitmap_len,len,rc);

	return len;
}

static void _rdata_nxt_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_nxt_t *nxt = (ch_dns_rdata_nxt_t*)rdata;
     ch_dns_name_t *name = &nxt->name;  

     ch_msgpack_store_map_start(dstore,"nxt",2);
     ch_dns_name_store(name,dstore);
     ch_msgpack_store_write_bin_kv(dstore,"tbtm",(void*)nxt->typebitmap,nxt->typebitmap_len);

}

static ch_dns_rdata_t * _rdata_nxt_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_nxt_t *nxt = (ch_dns_rdata_nxt_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_nxt_t));

	ch_dns_name_t *name = &nxt->name;

	nxt->rdata.rdata_dump = _rdata_nxt_dump;
	nxt->rdata.rdata_write = _rdata_nxt_write;
	nxt->rdata.rdata_store = _rdata_nxt_store;


	CH_DNS_NAME_INIT(name);
	
	nxt->typebitmap_len =  0;
	nxt->typebitmap = NULL;

	return (ch_dns_rdata_t*)nxt;

}

static int _rdata_nxt_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_nxt_t *nxt = (ch_dns_rdata_nxt_t*)rdata;
	
	if(rdata->dlen  == 0|| rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);
	
	if(ch_dns_rdata_name_parse(mp,rdata,&nxt->name)){
	
		ch_log(CH_LOG_ERR,"Parse rdata.nxt.name failed!");
		return -1;
	}

	nxt->typebitmap_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(nxt->typebitmap_len,din,-1);
	nxt->typebitmap = ch_dns_data_input_bytes_read(din,mp,nxt->typebitmap_len);

	return 0;
}


static ch_dns_rdata_parser_t nxt_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_nxt,
	_rdata_nxt_create,
	_rdata_nxt_parse
};


void ch_dns_rdata_nxt_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&nxt_parser);
}


