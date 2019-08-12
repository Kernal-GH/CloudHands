/*
 *
 *      Filename: ch_dns_rdata_gpos.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-13 12:01:16
 */


#include "ch_dns_rdata_gpos.h"
#include "ch_log.h"

static void _rdata_gpos_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_gpos_t *gpos = (ch_dns_rdata_gpos_t*)rdata;

	fprintf(fp,"Dump The rdata.gpos:\n");

	fprintf(fp,"rdata.gpos.longitude:%s\n",gpos->longitude);
	fprintf(fp,"rdata.gpos.latitude:%s\n",gpos->latitude);
	fprintf(fp,"rdata.gpos.altitude:%s\n",gpos->altitude);


}

static ssize_t  _rdata_gpos_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_gpos_t *gpos = (ch_dns_rdata_gpos_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *longitude = gpos->longitude;
	unsigned char *latitude = gpos->latitude;
	unsigned char *altitude = gpos->altitude;

	CH_DOUT_STRING16_WRITE2(dout,longitude,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,latitude,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,altitude,len,rc);

	return len;
}

static void _rdata_gpos_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_gpos_t *gpos = (ch_dns_rdata_gpos_t*)rdata;    

    ch_msgpack_store_map_start(dstore,"gpos",3);
    ch_msgpack_store_write_kv(dstore,"longitude",(const char*)gpos->longitude);
    ch_msgpack_store_write_kv(dstore,"latitude",(const char*)gpos->latitude);
    ch_msgpack_store_write_kv(dstore,"altitude",(const char*)gpos->altitude);

}

static ch_dns_rdata_t * _rdata_gpos_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_gpos_t *gpos = (ch_dns_rdata_gpos_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_gpos_t));

	gpos->rdata.rdata_dump = _rdata_gpos_dump;
	gpos->rdata.rdata_write = _rdata_gpos_write;
	gpos->rdata.rdata_store = _rdata_gpos_store;

	gpos->longitude = NULL;
	gpos->latitude = NULL;
	gpos->altitude = NULL;

	return (ch_dns_rdata_t*)gpos;

}

static int _rdata_gpos_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_gpos_t *gpos = (ch_dns_rdata_gpos_t*)rdata;
	
	if(rdata->dlen <3 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	gpos->longitude = ch_dns_data_input_bstring_read_dup(din,mp);
	gpos->latitude = ch_dns_data_input_bstring_read_dup(din,mp);
	gpos->altitude = ch_dns_data_input_bstring_read_dup(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t gpos_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_gpos,
	_rdata_gpos_create,
	_rdata_gpos_parse
};


void ch_dns_rdata_gpos_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&gpos_parser);
}


