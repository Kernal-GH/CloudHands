/*
 *
 *      Filename: ch_dns_rdata_naptr.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 17:11:51
 * Last Modified: 2018-06-13 18:25:20
 */

#include "ch_dns_rdata_naptr.h"
#include "ch_log.h"

static void _rdata_naptr_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_naptr_t *naptr = (ch_dns_rdata_naptr_t*)rdata;

	fprintf(fp,"Dump dns.rdata.naptr:\n");
	fprintf(fp,"dns.rdata.naptr.order:%u\n",naptr->order);
	fprintf(fp,"dns.rdata.naptr.preference:%u\n",naptr->preference);
	fprintf(fp,"dns.rdata.naptr.flags:%s\n",naptr->flags);
	fprintf(fp,"dns.rdata.naptr.service:%s\n",naptr->service);
	fprintf(fp,"dns.rdata.naptr.regex:%s\n",naptr->regex);

	ch_dns_name_dump(&naptr->replacement,fp);

}


static ssize_t  _rdata_naptr_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t rc,len = 0;
	ch_dns_rdata_naptr_t *naptr = (ch_dns_rdata_naptr_t*)rdata;
	ch_dns_name_t *name = &naptr->replacement;
	unsigned char *flags = naptr->flags;
	unsigned char *service = naptr->service;
	unsigned char *regex = naptr->regex;

	CH_DOUT_UINT16_WRITE(dout,naptr->order,len,rc);
	CH_DOUT_UINT16_WRITE(dout,naptr->preference,len,rc);
	
	CH_DOUT_STRING16_WRITE2(dout,flags,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,service,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,regex,len,rc);

	CH_DNS_NAME_WRITE(dout,name,len,rc);

	return len;

}

static void _rdata_naptr_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_naptr_t *naptr = (ch_dns_rdata_naptr_t*)rdata; 
    ch_dns_name_t *name = &naptr->replacement;
    const char *flags = (const char*)naptr->flags;
    const char *service = (const char*)naptr->service;
    const char *regex = (const char*)naptr->regex;

    ch_msgpack_store_map_start(dstore,"naptr",6);
    ch_msgpack_store_write_uint16(dstore,"order",naptr->order);
    ch_msgpack_store_write_uint16(dstore,"preference",naptr->preference);
    ch_msgpack_store_write_kv(dstore,"flags",flags);
    ch_msgpack_store_write_kv(dstore,"service",service);
    ch_msgpack_store_write_kv(dstore,"regex",regex);
    ch_dns_name_store(name,dstore);
}

static ch_dns_rdata_t * _rdata_naptr_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_naptr_t *naptr = (ch_dns_rdata_naptr_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_naptr_t));

	ch_dns_name_t *name = &naptr->replacement;


	naptr->rdata.rdata_dump = _rdata_naptr_dump;
	naptr->rdata.rdata_write = _rdata_naptr_write;
	naptr->rdata.rdata_store = _rdata_naptr_store;

	naptr->order = 0;
	naptr->preference = 0;

	naptr->flags = NULL;
	naptr->service = NULL;
	naptr->regex = NULL;

	CH_DNS_NAME_INIT(name);

	return (ch_dns_rdata_t*)naptr;

}

static int _rdata_naptr_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_naptr_t *naptr = (ch_dns_rdata_naptr_t*)rdata;
	ch_dns_data_input_t tmp,*din = &tmp;

	if(rdata->dlen <7 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	naptr->order = ch_dns_data_input_uint16_read(din);
	naptr->preference = ch_dns_data_input_uint16_read(din);

	naptr->flags = ch_dns_data_input_bstring_read_dup(din,mp); 
	naptr->service = ch_dns_data_input_bstring_read_dup(din,mp); 
	naptr->regex = ch_dns_data_input_bstring_read_dup(din,mp); 

	if(ch_dns_name_parse(mp,din,&naptr->replacement)){
	
		ch_log(CH_LOG_ERR,"Parse dns.rdata.naptr failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t naptr_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_naptr,
	_rdata_naptr_create,
	_rdata_naptr_parse
};


void ch_dns_rdata_naptr_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&naptr_parser);

}
