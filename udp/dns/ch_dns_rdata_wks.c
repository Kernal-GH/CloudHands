/*
 *
 *      Filename: ch_dns_rdata_wks.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 16:08:34
 * Last Modified: 2018-05-09 18:25:02
 */

#include "ch_dns_rdata_wks.h"
#include "ch_log.h"
#include "ch_net_util.h"

static void _rdata_wks_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	char b[64] = {0};

	ch_dns_rdata_wks_t *wks = (ch_dns_rdata_wks_t*)rdata;

	fprintf(fp,"Dump The rdata.wks:\n");
	ch_ip_to_str(b,64,wks->address);
	fprintf(fp,"rdata.wks.address:%s\n",b);
	fprintf(fp,"rdata.wks.protocol:%u\n",wks->protocol);
	fprintf(fp,"rdata.wks.maplen:%u\n",wks->map_len);
	fprintf(fp,"rdata.wks.bitmap:%s\n",(const char*)wks->bitmap);
}

static ssize_t  _rdata_wks_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_wks_t *wks = (ch_dns_rdata_wks_t*)rdata;

	if(-1 == (rc = ch_dout_uint32_write(dout,wks->address)))
		return -1;

	len += rc;

	if(-1 == (rc = ch_dout_uint8_write(dout,wks->protocol)))
		return -1;

	len+= rc;

	if(-1 == (rc = ch_dout_string16_write(dout,wks->bitmap,wks->map_len)))
		return -1;

	len+= rc;

	return len;
}

static ch_dns_rdata_t * _rdata_wks_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_wks_t *rdata = (ch_dns_rdata_wks_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_wks_t));

	rdata->address = 0;
	rdata->protocol = 0;
	rdata->map_len = 0;
	rdata->bitmap = NULL;

	rdata->rdata.rdata_dump = _rdata_wks_dump;
	rdata->rdata.rdata_write = _rdata_wks_write;

	return (ch_dns_rdata_t*)rdata;

}

static int _rdata_wks_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_wks_t *rdata_wks = (ch_dns_rdata_wks_t*)rdata;

	if(rdata->dlen <5 || rdata->data == NULL)
		return -1;

	ch_dns_data_input_t tmp,*din=&tmp;
	ch_dns_rdata_input_init(din,rdata);

	rdata_wks->address = ch_dns_data_input_uint32_read(din);
	rdata_wks->protocol = ch_dns_data_input_uint8_read(din);
	rdata_wks->map_len = ch_dns_data_input_rdlen(din);
	
	if(rdata_wks->map_len){
	
		rdata_wks->bitmap = ch_pcalloc(mp,rdata_wks->map_len);
		memcpy(rdata_wks->bitmap,ch_dns_data_input_pos(din),rdata_wks->map_len);
	}

	return 0;
}

static ch_dns_rdata_parser_t wks_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_wks,
	_rdata_wks_create,
	_rdata_wks_parse
};


void ch_dns_rdata_wks_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&wks_parser);

}


