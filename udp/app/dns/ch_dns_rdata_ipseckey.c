/*
 *
 *      Filename: ch_dns_rdata_ipseckey.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 13:50:20
 */


#include "ch_dns_rdata_ipseckey.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_ipseckey_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_ipseckey_t *ipseckey = (ch_dns_rdata_ipseckey_t*)rdata;

	char buffer[INET6_ADDRSTRLEN];  

	fprintf(fp,"Dump The rdata.ipseckey:\n");
	fprintf(fp,"rdata.ipseckey.precedence:%u\n",ipseckey->precedence);
	fprintf(fp,"rdata.ipseckey.gateway_type:%u\n",ipseckey->gateway_type);
	fprintf(fp,"rdata.ipseckey.alg:%u\n",ipseckey->alg);

	const char *gw ="";

	switch(ipseckey->gateway_type){
	
	case 0:
		gw ="none";
		break;

	case 1:
		gw = inet_ntoa(ipseckey->in_addr);
		break;

	case 2:
		if(inet_ntop(AF_INET6,(void*)(&ipseckey->in6_addr),buffer, INET6_ADDRSTRLEN) == NULL){    
        
			gw = "invalid ipv6 address!";   
		}else{

			gw = (const char*)buffer;
		} 	 
		
		break;

	case 3:
		gw = (const char*)ipseckey->gateway.ndata;	
		break;

	default:
		break;
	}
	fprintf(fp,"rdata.ipseckey.gateway:%s\n",gw);

}

static ssize_t  _rdata_ipseckey_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_ipseckey_t *ipseckey = (ch_dns_rdata_ipseckey_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *data = NULL;
	uint16_t dlen = 0;
	uint32_t v;

	CH_DOUT_UINT8_WRITE(dout,ipseckey->precedence,len,rc);
	CH_DOUT_UINT8_WRITE(dout,ipseckey->gateway_type,len,rc);
	CH_DOUT_UINT8_WRITE(dout,ipseckey->alg,len,rc);

	switch(ipseckey->gateway_type){
	
	case 0:

		break;

	case 1:
		v =  ntohl(ipseckey->in_addr.s_addr);
		CH_DOUT_UINT32_WRITE(dout,v,len,rc);

		break;

	case 2:
		
		data = (unsigned char*)ipseckey->in6_addr.s6_addr;
		dlen = 16;
		CH_DOUT_STRING16_WRITE(dout,data,dlen,len,rc);

		break;

	case 3:
		ch_dns_name_write(dout,&ipseckey->gateway);

		break;

	default:
		break;
	}

	data = ipseckey->key;
	dlen = ipseckey->key_len;

	CH_DOUT_STRING16_WRITE(dout,data,dlen,len,rc);

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


static void  _rdata_ipseckey_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

	ch_dns_rdata_ipseckey_t *ipseckey = (ch_dns_rdata_ipseckey_t*)rdata;

	uint32_t v;

    ch_msgpack_store_map_start(dstore,"ipseckey",5);
    ch_msgpack_store_write_uint8(dstore,"precedence",ipseckey->precedence);
    ch_msgpack_store_write_uint8(dstore,"gatewayType",ipseckey->gateway_type);
    ch_msgpack_store_write_uint8(dstore,"alg",ipseckey->alg);

	switch(ipseckey->gateway_type){
	
	case 1:
		v =  ntohl(ipseckey->in_addr.s_addr);
        uint8_t barr[4]={0};
        _VALUE_WRITE(barr,v,4);

        ch_msgpack_store_write_bin_kv(dstore,"addr4",(void*)barr,4);
		break;

	case 2:
		
        ch_msgpack_store_write_bin_kv(dstore,"addr6",(void*)ipseckey->in6_addr.s6_addr,16);

		break;

	case 3:
		ch_dns_name_store(&ipseckey->gateway,dstore);
		break;

	default:
        ch_msgpack_store_write_uint8(dstore,"unknown",1);
        break;
	}

    ch_msgpack_store_write_bin_kv(dstore,"key",(void*)ipseckey->key,ipseckey->key_len);
}

static ch_dns_rdata_t * _rdata_ipseckey_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_ipseckey_t *ipseckey = (ch_dns_rdata_ipseckey_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_ipseckey_t));

	ipseckey->rdata.rdata_dump = _rdata_ipseckey_dump;
	ipseckey->rdata.rdata_write = _rdata_ipseckey_write;
	ipseckey->rdata.rdata_store = _rdata_ipseckey_store;

	ipseckey->precedence = 0;
	ipseckey->gateway_type = 0;
	ipseckey->alg = 0;
	CH_DNS_NAME_INIT(&ipseckey->gateway);

	ipseckey->key_len = 0;
	ipseckey->key = NULL;
	ipseckey->in_addr.s_addr = 0;

	return (ch_dns_rdata_t*)ipseckey;

}

static int _rdata_ipseckey_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_ipseckey_t *ipseckey = (ch_dns_rdata_ipseckey_t*)rdata;
	
	if(rdata->dlen <3 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	ipseckey->precedence = ch_dns_data_input_uint8_read(din);
	ipseckey->gateway_type = ch_dns_data_input_uint8_read(din);
	ipseckey->alg = ch_dns_data_input_uint8_read(din);

	switch(ipseckey->gateway_type){
	
	case 0:

		break;

	case 1:
		ipseckey->in_addr.s_addr = htonl(ch_dns_data_input_uint32_read(din));
		break;

	case 2:
		memcpy(ipseckey->in6_addr.s6_addr,din->pos,16);
		ch_dns_data_input_pos_update(din,16);
		break;

	case 3:
		ch_dns_name_parse(mp,din,&ipseckey->gateway);
		break;

	default:
		break;
	}

	ipseckey->key_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(ipseckey->key_len,din,-1);

	ipseckey->key = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t ipseckey_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_ipseckey,
	_rdata_ipseckey_create,
	_rdata_ipseckey_parse
};


void ch_dns_rdata_ipseckey_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&ipseckey_parser);
}


