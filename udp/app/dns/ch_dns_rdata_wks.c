/*
 *
 *      Filename: ch_dns_rdata_wks.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-03 16:08:34
 * Last Modified: 2018-06-26 14:04:23
 */

#include "ch_dns_rdata_wks.h"
#include "ch_log.h"
#include "ch_net_util.h"
#include "ch_util.h"

static void _rdata_wks_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	char b[64] = {0};

	ch_dns_rdata_wks_t *wks = (ch_dns_rdata_wks_t*)rdata;

	fprintf(fp,"Dump The rdata.wks:\n");
	ch_ip_to_str(b,64,wks->address);
	fprintf(fp,"rdata.wks.address:%s\n",b);
	fprintf(fp,"rdata.wks.protocol:%u\n",wks->protocol);
	
	ch_hex_string_dump("rdata.wks.bitmap",wks->bitmap,wks->map_len,fp);

}

static ssize_t  _rdata_wks_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ssize_t len = 0,rc;
	ch_dns_rdata_wks_t *wks = (ch_dns_rdata_wks_t*)rdata;

	CH_DOUT_UINT32_WRITE(dout,wks->address,len,rc);
	CH_DOUT_UINT8_WRITE(dout,wks->protocol,len,rc);

	CH_DOUT_STRING16_WRITE(dout,wks->bitmap,wks->map_len,len,rc);

	return len;
}

#define _VALUE_WRITE(buf,value,byte_num) 		\
	do{												\
		size_t bytes,i;								\
		bytes = (byte_num)*8;						\
		for(i=1;i<=(byte_num);i++)					\
		{											\
			buf[i-1]=((value)>>(bytes-i*8))&0xff;	\
		}											\
	}while(0)

static void _rdata_wks_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_wks_t *wks = (ch_dns_rdata_wks_t*)rdata;  

    ch_msgpack_store_map_start(dstore,"wks",3);
    
    uint8_t barr[4]={0};
    _VALUE_WRITE(barr,wks->address,4);
    
    ch_msgpack_store_write_bin_kv(dstore,"addr",(void*)barr,4);
    ch_msgpack_store_write_uint8(dstore,"protocol",wks->protocol);
    ch_msgpack_store_write_bin_kv(dstore,"bitmap",(void*)wks->bitmap,wks->map_len);

}

static ch_dns_rdata_t * _rdata_wks_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_wks_t *wks = (ch_dns_rdata_wks_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_wks_t));
	
	wks->rdata.rdata_dump = _rdata_wks_dump;
	wks->rdata.rdata_write = _rdata_wks_write;
	wks->rdata.rdata_store = _rdata_wks_store;

	wks->address = 0;
	wks->protocol = 0;
	wks->map_len = 0;
	wks->bitmap = NULL;

	return (ch_dns_rdata_t*)wks;

}

static int _rdata_wks_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_rdata_wks_t *wks = (ch_dns_rdata_wks_t*)rdata;

	if(rdata->dlen <5 || rdata->data == NULL)
		return -1;

	ch_dns_data_input_t tmp,*din=&tmp;
	ch_dns_rdata_input_init(din,rdata);

	wks->address = ch_dns_data_input_uint32_read(din);
	wks->protocol = ch_dns_data_input_uint8_read(din);
	
	wks->map_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(wks->map_len,din,-1);
	
	if(wks->map_len){

		wks->bitmap = ch_dns_data_input_bytes_read(din,mp,wks->map_len);
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


