/*
 *
 *      Filename: ch_dns_rdata_nsec3param.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 14:07:28
 */


#include "ch_dns_rdata_nsec3param.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_nsec3param_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_nsec3param_t *nsec3param = (ch_dns_rdata_nsec3param_t*)rdata;

	fprintf(fp,"Dump The rdata.nsec3param:\n");
	fprintf(fp,"rdata.nsec3param.hash_alg:%u\n",nsec3param->hash_alg);
	fprintf(fp,"rdata.nsec3param.flags:%u\n",nsec3param->flags);
	fprintf(fp,"rdata.nsec3param.iterations:%u\n",nsec3param->iterations);

	if(nsec3param->salt_len){
	
		ch_hex_string_dump("rdata.nsec3param.salt",nsec3param->salt,nsec3param->salt_len,fp);
	}else{
	
		fprintf(fp,"rdata.nsec3param.salt:none\n");
	}

}

static ssize_t  _rdata_nsec3param_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_nsec3param_t *nsec3param = (ch_dns_rdata_nsec3param_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *salt = nsec3param->salt;
	uint16_t salt_len = nsec3param->salt_len;

	CH_DOUT_UINT8_WRITE(dout,nsec3param->hash_alg,len,rc);
	CH_DOUT_UINT8_WRITE(dout,nsec3param->flags,len,rc);
	CH_DOUT_UINT16_WRITE(dout,nsec3param->iterations,len,rc);

	CH_DOUT_STRING16_WRITE(dout,salt,salt_len,len,rc);

	return len;
}

static void _rdata_nsec3param_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_nsec3param_t *nsec3param = (ch_dns_rdata_nsec3param_t*)rdata;
     ch_msgpack_store_map_start(dstore,"nsec3param",4);
     ch_msgpack_store_write_uint8(dstore,"alg",nsec3param->hash_alg);
     ch_msgpack_store_write_uint8(dstore,"flags",nsec3param->flags);
     ch_msgpack_store_write_uint16(dstore,"iterations",nsec3param->iterations);
     ch_msgpack_store_write_bin_kv(dstore,"salt",(void*)nsec3param->salt,nsec3param->salt_len);

}

static ch_dns_rdata_t * _rdata_nsec3param_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_nsec3param_t *nsec3param = (ch_dns_rdata_nsec3param_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_nsec3param_t));

	nsec3param->rdata.rdata_dump = _rdata_nsec3param_dump;
	nsec3param->rdata.rdata_write = _rdata_nsec3param_write;
	nsec3param->rdata.rdata_store = _rdata_nsec3param_store;

	nsec3param->hash_alg = 0;
	nsec3param->flags = 0;
	nsec3param->iterations = 0;
	nsec3param->salt_len = 0;
	nsec3param->salt = NULL;

	return (ch_dns_rdata_t*)nsec3param;

}

static int _rdata_nsec3param_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_nsec3param_t *nsec3param = (ch_dns_rdata_nsec3param_t*)rdata;
	
	if(rdata->dlen <5 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	nsec3param->hash_alg = ch_dns_data_input_uint8_read(din);
	nsec3param->flags = ch_dns_data_input_uint8_read(din);
	nsec3param->iterations = ch_dns_data_input_uint16_read(din);
	
	nsec3param->salt_len = ch_dns_data_input_uint8_read(din);
	CH_DNS_DLEN_CHECK((uint16_t)nsec3param->salt_len,din,-1);

	if(nsec3param->salt_len){
	
		nsec3param->salt = ch_dns_data_input_bytes_read(din,mp,nsec3param->salt_len);
	}

	return 0;
}


static ch_dns_rdata_parser_t nsec3param_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_nsec3param,
	_rdata_nsec3param_create,
	_rdata_nsec3param_parse
};


void ch_dns_rdata_nsec3param_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&nsec3param_parser);
}


