/*
 *
 *      Filename: ch_dns_rdata_px.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 10:23:52
 * Last Modified: 2018-06-14 17:46:26
 */

#include "ch_dns_rdata_px.h"
#include "ch_log.h"

static void _rdata_px_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_px_t *px = (ch_dns_rdata_px_t*)rdata;

	fprintf(fp,"Dump The rdata.px:\n");
	fprintf(fp,"rdata.px.preference:%u\n",px->preference);
	fprintf(fp,"rdata.px.map822:\n");
	ch_dns_name_dump(&px->map822,fp);

	fprintf(fp,"rdata.px.mapx400:\n");
	ch_dns_name_dump(&px->mapx400,fp);

}

static ssize_t  _rdata_px_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;

	ch_dns_rdata_px_t *px = (ch_dns_rdata_px_t*)rdata;
	ch_dns_name_t *map822 = &px->map822;
	ch_dns_name_t *mapx400 = &px->mapx400;

	CH_DOUT_UINT16_WRITE(dout,px->preference,len,rc);
	CH_DNS_NAME_WRITE(dout,map822,len,rc);
	CH_DNS_NAME_WRITE(dout,mapx400,len,rc);

	return len;
}

static void _rdata_px_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_px_t *px = (ch_dns_rdata_px_t*)rdata; 
    ch_dns_name_t *map822 = &px->map822;
    ch_dns_name_t *mapx400 = &px->mapx400;  

    ch_msgpack_store_map_start(dstore,"px",3);
    ch_msgpack_store_write_uint16(dstore,"preference",px->preference);
    ch_dns_name_store_wkey(map822,dstore,"map822");
    ch_dns_name_store_wkey(mapx400,dstore,"mapx400");

}

static ch_dns_rdata_t * _rdata_px_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_px_t *px = (ch_dns_rdata_px_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_px_t));

	ch_dns_name_t *map822 = &px->map822;
	ch_dns_name_t *mapx400 = &px->mapx400;



	px->rdata.rdata_dump = _rdata_px_dump;
	px->rdata.rdata_write = _rdata_px_write;
	px->rdata.rdata_store = _rdata_px_store;
	
	px->preference = 0;
	CH_DNS_NAME_INIT(map822);
	CH_DNS_NAME_INIT(mapx400);

	return (ch_dns_rdata_t*)px;

}

static int _rdata_px_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din = &tmp;

	ch_dns_rdata_px_t *px = (ch_dns_rdata_px_t*)rdata;
	
	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	px->preference = ch_dns_data_input_uint16_read(din);

	if(ch_dns_name_parse(mp,din,&px->map822)){
	
		ch_log(CH_LOG_ERR,"Parse px.name.map822 failed!");
		return -1;
	}
	
	if(ch_dns_name_parse(mp,din,&px->mapx400)){
	
		ch_log(CH_LOG_ERR,"Parse px.name.mapx400 failed!");
		return -1;
	}

	return 0;
}


static ch_dns_rdata_parser_t px_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_px,
	_rdata_px_create,
	_rdata_px_parse
};


void ch_dns_rdata_px_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&px_parser);

}

