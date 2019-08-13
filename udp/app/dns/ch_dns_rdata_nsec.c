/*
 *
 *      Filename: ch_dns_rdata_nsec.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:55:27
 */


#include "ch_dns_rdata_nsec.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_nsec_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_nsec_t *nsec = (ch_dns_rdata_nsec_t*)rdata;

	fprintf(fp,"Dump The rdata.nsec:\n");

	ch_dns_name_dump(&nsec->name,fp);

	ch_hex_string_dump("rdata.nsec.typebitmap",nsec->typebitmap,nsec->typebitmap_len,fp);

}

static ssize_t  _rdata_nsec_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_nsec_t *nsec = (ch_dns_rdata_nsec_t*)rdata;
	ssize_t rc,len = 0;

	ch_dns_name_t *name = &nsec->name;

	CH_DNS_NAME_WRITE(dout,name,len,rc);

	CH_DOUT_STRING16_WRITE(dout,nsec->typebitmap,nsec->typebitmap_len,len,rc);

	return len;
}

static void _rdata_nsec_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_nsec_t *nsec = (ch_dns_rdata_nsec_t*)rdata; 
     ch_dns_name_t *name = &nsec->name; 

     ch_msgpack_store_map_start(dstore,"nsec",2);
     
     ch_dns_name_store(name,dstore);
     ch_msgpack_store_write_bin_kv(dstore,"tbtm",(void*)nsec->typebitmap,nsec->typebitmap_len);
     
}

static ch_dns_rdata_t * _rdata_nsec_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_nsec_t *nsec = (ch_dns_rdata_nsec_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_nsec_t));

	ch_dns_name_t *name = &nsec->name;

	nsec->rdata.rdata_dump = _rdata_nsec_dump;
	nsec->rdata.rdata_write = _rdata_nsec_write;
	nsec->rdata.rdata_store = _rdata_nsec_store;


	CH_DNS_NAME_INIT(name);
	
	nsec->typebitmap_len =  0;
	nsec->typebitmap = NULL;

	return (ch_dns_rdata_t*)nsec;

}

static int _rdata_nsec_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_nsec_t *nsec = (ch_dns_rdata_nsec_t*)rdata;
	
	if(rdata->dlen  == 0|| rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);
	
	if(ch_dns_rdata_name_parse(mp,rdata,&nsec->name)){
	
		ch_log(CH_LOG_ERR,"Parse rdata.nsec.name failed!");
		return -1;
	}

	nsec->typebitmap_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(nsec->typebitmap_len,din,-1);
	nsec->typebitmap = ch_dns_data_input_bytes_read(din,mp,nsec->typebitmap_len);

	return 0;
}


static ch_dns_rdata_parser_t nsec_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_nsec,
	_rdata_nsec_create,
	_rdata_nsec_parse
};


void ch_dns_rdata_nsec_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&nsec_parser);
}


