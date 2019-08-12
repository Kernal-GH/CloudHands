/*
 *
 *      Filename: ch_dns_rdata_kx.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 17:11:51
 * Last Modified: 2018-06-13 17:11:43
 */

#include "ch_dns_rdata_kx.h"
#include "ch_log.h"

static void _rdata_kx_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_kx_t *kx = (ch_dns_rdata_kx_t*)rdata;

	fprintf(fp,"Dump The KX Rdata:\n");
	fprintf(fp,"dns.rdata.kx.preference:%u\n",kx->preference);

	ch_dns_name_dump(&kx->name,fp);

}

static ssize_t  _rdata_kx_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t rc,len = 0;
	ch_dns_rdata_kx_t *kx = (ch_dns_rdata_kx_t*)rdata;
	ch_dns_name_t *name = &kx->name;

	CH_DOUT_UINT16_WRITE(dout,kx->preference,len,rc);
	
	CH_DNS_NAME_WRITE(dout,name,len,rc);

	return len;

}

static void _rdata_kx_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_kx_t *kx = (ch_dns_rdata_kx_t*)rdata;
    ch_msgpack_store_map_start(dstore,"kx",2);
    ch_msgpack_store_write_uint16(dstore,"preference",kx->preference);
    ch_dns_name_store(&kx->name,dstore);
}

static ch_dns_rdata_t * _rdata_kx_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_kx_t *kx = (ch_dns_rdata_kx_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_kx_t));

	ch_dns_name_t *name = &kx->name;


	kx->rdata.rdata_dump = _rdata_kx_dump;
	kx->rdata.rdata_write = _rdata_kx_write;
	kx->rdata.rdata_store = _rdata_kx_store;

	kx->preference = 0;
	CH_DNS_NAME_INIT(name);

	return (ch_dns_rdata_t*)kx;

}

static int _rdata_kx_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_kx_t *kx = (ch_dns_rdata_kx_t*)rdata;
	ch_dns_data_input_t tmp,*din = &tmp;

	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);
	kx->preference = ch_dns_data_input_uint16_read(din);

	if(ch_dns_name_parse(mp,din,&kx->name)){
	
		ch_log(CH_LOG_ERR,"Parse dns.rdata.kx failed!");
		return -1;
	}

	return 0;
}

static ch_dns_rdata_parser_t kx_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_kx,
	_rdata_kx_create,
	_rdata_kx_parse
};


void ch_dns_rdata_kx_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&kx_parser);

}
