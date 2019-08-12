/*
 *
 *      Filename: ch_dns_rdata_isdn.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-02 18:06:02
 * Last Modified: 2018-06-13 16:37:33
 */

#include "ch_dns_rdata_isdn.h"
#include "ch_log.h"

static void _rdata_isdn_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_isdn_t *isdn = (ch_dns_rdata_isdn_t*)rdata;

	fprintf(fp,"Dump The  rdata.isdn:\n");
	fprintf(fp,"rdata.isdn.address:%s\n",isdn->addr);
	fprintf(fp,"rdata.isdn.subaddress:%s\n",isdn->subaddr);

}

static ssize_t  _rdata_isdn_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_isdn_t *isdn = (ch_dns_rdata_isdn_t*)rdata;

	unsigned char *addr = isdn->addr;
	unsigned char *subaddr = isdn->subaddr;


	CH_DOUT_STRING16_WRITE2(dout,addr,len,rc);
	CH_DOUT_STRING16_WRITE2(dout,subaddr,len,rc);
	
	return len;

}

static void _rdata_isdn_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_isdn_t *isdn = (ch_dns_rdata_isdn_t*)rdata;
    ch_msgpack_store_map_start(dstore,"isdn",2);
    ch_msgpack_store_write_kv(dstore,"addr",(const char*)isdn->addr);
    ch_msgpack_store_write_kv(dstore,"subaddr",(const char*)isdn->subaddr);

}

static ch_dns_rdata_t * _rdata_isdn_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_isdn_t *isdn = (ch_dns_rdata_isdn_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_isdn_t));



	isdn->rdata.rdata_dump = _rdata_isdn_dump;
	isdn->rdata.rdata_write = _rdata_isdn_write;
	isdn->rdata.rdata_store = _rdata_isdn_store;
	
	isdn->addr = NULL;
	isdn->subaddr = NULL;

	return (ch_dns_rdata_t*)isdn;

}

static int _rdata_isdn_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_isdn_t *isdn = (ch_dns_rdata_isdn_t*)rdata;
	ch_dns_data_input_t tmp,*din = &tmp;

	if(rdata->dlen <1 || rdata->data == NULL)
		return -1;
	
	ch_dns_rdata_input_init(din,rdata);

	isdn->addr = ch_dns_data_input_bstring_read_dup(din,mp);

	isdn->subaddr =  ch_dns_data_input_bstring_read_dup(din,mp);

	return 0;
}

static ch_dns_rdata_parser_t isdn_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_isdn,
	_rdata_isdn_create,
	_rdata_isdn_parse
};


void ch_dns_rdata_isdn_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&isdn_parser);

}

