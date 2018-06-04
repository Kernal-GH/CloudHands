/*
 *
 *      Filename: ch_dns_rdata_hinfo.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 18:06:02
 * Last Modified: 2018-05-09 12:15:40
 */

#include "ch_dns_rdata_hinfo.h"
#include "ch_log.h"

static void _rdata_hinfo_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_hinfo_t *hinfo = (ch_dns_rdata_hinfo_t*)rdata;

	fprintf(fp,"Dump The  rdata.hinfo:\n");
	fprintf(fp,"rdata.hinfo.cpu:%s\n",hinfo->cpu?hinfo->cpu:"");
	fprintf(fp,"rdata.hinfo.os:%s\n",hinfo->os?hinfo->os:"");

}

static ssize_t  _rdata_hinfo_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_hinfo_t *hinfo = (ch_dns_rdata_hinfo_t*)rdata;

	if( -1 == (rc =ch_dout_string16_write(dout,hinfo->cpu!=NULL?(unsigned char*)hinfo->cpu:(unsigned char*)"",hinfo->cpu?strlen(hinfo->cpu):0)))
		return -1;

	len+=rc;
	
	if( -1 == (rc =ch_dout_string16_write(dout,hinfo->os!=NULL?(unsigned char*)hinfo->os:(unsigned char*)"",hinfo->os?strlen(hinfo->os):0)))
		return -1;

	len+=rc;

	return len;

}

static ch_dns_rdata_t * _rdata_hinfo_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_hinfo_t *rdata = (ch_dns_rdata_hinfo_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_hinfo_t));

	rdata->cpu = NULL;
	rdata->os = NULL;

	rdata->rdata.rdata_dump = _rdata_hinfo_dump;
	rdata->rdata.rdata_write = _rdata_hinfo_write;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_hinfo_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_hinfo_t *rdata_hinfo = (ch_dns_rdata_hinfo_t*)rdata;
	ch_dns_data_input_t din;

	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;
	
	ch_dns_rdata_input_init(&din,rdata);

	rdata_hinfo->cpu = ch_dns_data_input_bstring_read_dup(&din,mp);

	rdata_hinfo->os =  ch_dns_data_input_bstring_read_dup(&din,mp);

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

