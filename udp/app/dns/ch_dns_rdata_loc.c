/*
 *
 *      Filename: ch_dns_rdata_loc.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-13 17:33:58
 */


#include "ch_dns_rdata_loc.h"
#include "ch_log.h"

static void _rdata_loc_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_loc_t *loc = (ch_dns_rdata_loc_t*)rdata;

	fprintf(fp,"Dump The rdata.loc:\n");

	fprintf(fp,"rdata.loc.version:%u\n",loc->version);
	fprintf(fp,"rdata.loc.size:%u\n",loc->size);
	fprintf(fp,"rdata.loc.hprecision:%u\n",loc->hprecision);
	fprintf(fp,"rdata.loc.vprecision:%u\n",loc->vprecision);
	fprintf(fp,"rdata.loc.latitude:%lu\n",(unsigned long)loc->latitude);
	fprintf(fp,"rdata.loc.longitude:%lu\n",(unsigned long)loc->longitude);
	fprintf(fp,"rdata.loc.altitude:%lu\n",(unsigned long)loc->altitude);


}

static ssize_t  _rdata_loc_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_loc_t *loc = (ch_dns_rdata_loc_t*)rdata;
	ssize_t rc,len = 0;

	CH_DOUT_UINT8_WRITE(dout,loc->version,len,rc);
	CH_DOUT_UINT8_WRITE(dout,loc->size,len,rc);
	CH_DOUT_UINT8_WRITE(dout,loc->hprecision,len,rc);
	CH_DOUT_UINT8_WRITE(dout,loc->vprecision,len,rc);

	CH_DOUT_UINT32_WRITE(dout,loc->latitude,len,rc);
	CH_DOUT_UINT32_WRITE(dout,loc->longitude,len,rc);
	CH_DOUT_UINT32_WRITE(dout,loc->altitude,len,rc);


	return len;
}

static void _rdata_loc_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_loc_t *loc = (ch_dns_rdata_loc_t*)rdata;

    ch_msgpack_store_map_start(dstore,"loc",7);
    ch_msgpack_store_write_uint8(dstore,"version",loc->version);
    ch_msgpack_store_write_uint8(dstore,"size",loc->size);
    ch_msgpack_store_write_uint8(dstore,"hprecision",loc->hprecision);
    ch_msgpack_store_write_uint8(dstore,"vprecision",loc->vprecision);
    ch_msgpack_store_write_uint32(dstore,"latitude",loc->latitude);
    ch_msgpack_store_write_uint32(dstore,"longitude",loc->longitude);
    ch_msgpack_store_write_uint32(dstore,"altitude",loc->altitude);
}

static ch_dns_rdata_t * _rdata_loc_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_loc_t *loc = (ch_dns_rdata_loc_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_loc_t));

	loc->rdata.rdata_dump = _rdata_loc_dump;
	loc->rdata.rdata_write = _rdata_loc_write;
	loc->rdata.rdata_store = _rdata_loc_store;
	
	loc->version = 0;
	loc->size = 0;
	loc->hprecision = 0;
	loc->vprecision = 0;
	loc->latitude = 0;
	loc->longitude = 0;
	loc->altitude = 0;

	return (ch_dns_rdata_t*)loc;

}

static int _rdata_loc_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	/*unused*/
	mp = mp;

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_loc_t *loc = (ch_dns_rdata_loc_t*)rdata;
	
	if(rdata->dlen <16 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	loc->version = ch_dns_data_input_uint8_read(din);
	loc->size = ch_dns_data_input_uint8_read(din);
	loc->hprecision = ch_dns_data_input_uint8_read(din);
	loc->vprecision = ch_dns_data_input_uint8_read(din);

	loc->latitude = ch_dns_data_input_uint32_read(din);
	loc->longitude = ch_dns_data_input_uint32_read(din);
	loc->altitude = ch_dns_data_input_uint32_read(din);

	return 0;
}


static ch_dns_rdata_parser_t loc_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_loc,
	_rdata_loc_create,
	_rdata_loc_parse
};


void ch_dns_rdata_loc_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&loc_parser);
}


