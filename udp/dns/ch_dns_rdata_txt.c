/*
 *
 *      Filename: ch_dns_rdata_txt.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 19:46:10
 * Last Modified: 2018-06-13 12:10:32
 */

#include "ch_dns_rdata_txt.h"
#include "ch_log.h"


static void _rdata_txt_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_txt_t *txt = (ch_dns_rdata_txt_t*)rdata;
	unsigned char ** texts,*str;
	int i;

	fprintf(fp,"Dump The rdata.txt:\n");

	texts = (unsigned char**)txt->txts->elts;

	for(i = 0;i<txt->txts->nelts;i++){
	
		str = texts[i];
		fprintf(fp,"%s\n",str);
	}

}

static ssize_t  _rdata_txt_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_txt_t *txt = (ch_dns_rdata_txt_t*)rdata;
	ssize_t rc,len = 0;
	unsigned char ** texts,*str;
	int i;

	texts = (unsigned char**)txt->txts->elts;
	
	for(i = 0;i<txt->txts->nelts;i++){
	
		str = texts[i];
		CH_DOUT_STRING16_WRITE2(dout,str,len,rc);
	}

	return len;
}

static ch_dns_rdata_t * _rdata_txt_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_txt_t *rdata = (ch_dns_rdata_txt_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_txt_t));

	rdata->txts = ch_array_make(mp,8,sizeof(unsigned char*));

	rdata->rdata.rdata_dump = _rdata_txt_dump;
	rdata->rdata.rdata_write = _rdata_txt_write;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_txt_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	unsigned char *txt;

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_txt_t *rdata_txt = (ch_dns_rdata_txt_t*)rdata;
	
	if(rdata->dlen == 0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	while(!ch_dns_data_input_read_over(din,1)){
	
		txt = ch_dns_data_input_bstring_read_dup(din,mp);
		if(txt == NULL)
			break;

		ch_dns_rdata_txt_add(rdata_txt,txt);
	}

	return 0;
}

static ch_dns_rdata_parser_t txt_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_txt,
	_rdata_txt_create,
	_rdata_txt_parse
};


void ch_dns_rdata_txt_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&txt_parser);

}


