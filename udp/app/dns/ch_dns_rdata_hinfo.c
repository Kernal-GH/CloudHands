/*
 *
 *      Filename: ch_dns_rdata_hinfo.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 18:06:02
 * Last Modified: 2018-06-13 16:39:37
 */

#include "ch_dns_rdata_hinfo.h"
#include "ch_log.h"

static void _rdata_hinfo_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_hinfo_t *hinfo = (ch_dns_rdata_hinfo_t*)rdata;

	fprintf(fp,"Dump The  rdata.hinfo:\n");
	fprintf(fp,"rdata.hinfo.cpu:%s\n",hinfo->cpu);
	fprintf(fp,"rdata.hinfo.os:%s\n",hinfo->os);

}

static ssize_t  _rdata_hinfo_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_hinfo_t *hinfo = (ch_dns_rdata_hinfo_t*)rdata;

	unsigned char *cpu = hinfo->cpu;
	unsigned char *os = hinfo->os;


	CH_DOUT_STRING16_WRITE2(dout,cpu,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,os,len,rc);
	
	return len;

}

static void _rdata_hinfo_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore) {

    ch_dns_rdata_hinfo_t *hinfo = (ch_dns_rdata_hinfo_t*)rdata;
    ch_msgpack_store_map_start(dstore,"hinfo",2);
    ch_msgpack_store_write_kv(dstore,"cpu",(const char*)hinfo->cpu);
    ch_msgpack_store_write_kv(dstore,"os",(const char*)hinfo->os);

}

static ch_dns_rdata_t * _rdata_hinfo_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_hinfo_t *hinfo = (ch_dns_rdata_hinfo_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_hinfo_t));

	hinfo->cpu = NULL;
	hinfo->os = NULL;

	hinfo->rdata.rdata_dump = _rdata_hinfo_dump;
	hinfo->rdata.rdata_write = _rdata_hinfo_write;
	hinfo->rdata.rdata_store = _rdata_hinfo_store;

	return (ch_dns_rdata_t*)hinfo;

}

static int _rdata_hinfo_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_hinfo_t *hinfo = (ch_dns_rdata_hinfo_t*)rdata;
	ch_dns_data_input_t tmp,*din = &tmp;

	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;
	
	ch_dns_rdata_input_init(din,rdata);

	hinfo->cpu = ch_dns_data_input_bstring_read_dup(din,mp);

	hinfo->os =  ch_dns_data_input_bstring_read_dup(din,mp);

	return 0;
}

static ch_dns_rdata_parser_t hinfo_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_hinfo,
	_rdata_hinfo_create,
	_rdata_hinfo_parse
};


void ch_dns_rdata_hinfo_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&hinfo_parser);

}

