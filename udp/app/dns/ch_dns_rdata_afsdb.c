/*
 *
 *      Filename: ch_dns_rdata_afsdb.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 17:11:51
 * Last Modified: 2018-06-15 15:14:32
 */

#include "ch_dns_rdata_afsdb.h"
#include "ch_log.h"

static void _rdata_afsdb_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_afsdb_t *afsdb = (ch_dns_rdata_afsdb_t*)rdata;

	fprintf(fp,"Dump The KX Rdata:\n");
	fprintf(fp,"dns.rdata.afsdb.subtype:%u\n",afsdb->subtype);
	fprintf(fp,"dns.rdata.afsdb.host:\n");
	ch_dns_name_dump(&afsdb->host,fp);

}

static ssize_t  _rdata_afsdb_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t rc,len = 0;
	ch_dns_rdata_afsdb_t *afsdb = (ch_dns_rdata_afsdb_t*)rdata;
	ch_dns_name_t *host = &afsdb->host;

	CH_DOUT_UINT16_WRITE(dout,afsdb->subtype,len,rc);
	
	CH_DNS_NAME_WRITE(dout,host,len,rc);

	return len;

}

static void _rdata_afsdb_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore) {

	ch_dns_rdata_afsdb_t *afsdb = (ch_dns_rdata_afsdb_t*)rdata;
	ch_dns_name_t *host = &afsdb->host;

    ch_msgpack_store_map_start(dstore,"afsdb",2);
    ch_msgpack_store_write_uint16(dstore,"subtype",afsdb->subtype);
    ch_dns_name_store(host,dstore);

}

static ch_dns_rdata_t * _rdata_afsdb_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_afsdb_t *afsdb = (ch_dns_rdata_afsdb_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_afsdb_t));

	ch_dns_name_t *host = &afsdb->host;


	afsdb->rdata.rdata_dump = _rdata_afsdb_dump;
	afsdb->rdata.rdata_write = _rdata_afsdb_write;
    afsdb->rdata.rdata_store = _rdata_afsdb_store;
	afsdb->subtype = 0;
	CH_DNS_NAME_INIT(host);

	return (ch_dns_rdata_t*)afsdb;

}

static int _rdata_afsdb_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_afsdb_t *afsdb = (ch_dns_rdata_afsdb_t*)rdata;
	ch_dns_data_input_t tmp,*din = &tmp;

	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);
	afsdb->subtype = ch_dns_data_input_uint16_read(din);

	CH_DNS_NAME_PARSE(mp,din,&afsdb->host);

	return 0;
}

static ch_dns_rdata_parser_t afsdb_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_afsdb,
	_rdata_afsdb_create,
	_rdata_afsdb_parse
};


void ch_dns_rdata_afsdb_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&afsdb_parser);

}
