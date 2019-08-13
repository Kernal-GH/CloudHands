/*
 *
 *      Filename: ch_dns_rdata_dhcid.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:47:05
 */


#include "ch_dns_rdata_dhcid.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_dhcid_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	char *decode = NULL;

	ch_dns_rdata_dhcid_t *dhcid = (ch_dns_rdata_dhcid_t*)rdata;

	fprintf(fp,"Dump The rdata.dhcid:\n");
	
	if(dhcid->dlen){
	
		decode = malloc(dhcid->dlen+1);
		if(decode){
		
			ch_decode_base64_ext(decode,(const unsigned char*)dhcid->data,dhcid->dlen);
		}

	}

	fprintf(fp,"rdata.dhcid.data:%s\n",decode?decode:"");
	if(decode){
	
		free(decode);
	}

}

static ssize_t  _rdata_dhcid_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_dhcid_t *dhcid = (ch_dns_rdata_dhcid_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *data = dhcid->data;
	uint16_t dlen = dhcid->dlen;

	CH_DOUT_STRING16_WRITE(dout,data,dlen,len,rc);

	return len;
}

static void _rdata_dhcid_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_dhcid_t *dhcid = (ch_dns_rdata_dhcid_t*)rdata; 
     ch_msgpack_store_map_start(dstore,"dhcid",1);
     ch_msgpack_store_write_bin_kv(dstore,"data",(void*)dhcid->data,dhcid->dlen);

}

static ch_dns_rdata_t * _rdata_dhcid_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_dhcid_t *dhcid = (ch_dns_rdata_dhcid_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_dhcid_t));

	dhcid->rdata.rdata_dump = _rdata_dhcid_dump;
	dhcid->rdata.rdata_write = _rdata_dhcid_write;
    dhcid->rdata.rdata_store = _rdata_dhcid_store;
	dhcid->dlen = 0;
	dhcid->data = NULL;

	return (ch_dns_rdata_t*)dhcid;

}

static int _rdata_dhcid_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_dhcid_t *dhcid = (ch_dns_rdata_dhcid_t*)rdata;
	
	if(rdata->dlen  == 0 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	dhcid->dlen = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(dhcid->dlen,din,-1);

	dhcid->data = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t dhcid_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_dhcid,
	_rdata_dhcid_create,
	_rdata_dhcid_parse
};


void ch_dns_rdata_dhcid_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&dhcid_parser);
}


