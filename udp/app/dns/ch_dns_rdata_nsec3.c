/*
 *
 *      Filename: ch_dns_rdata_nsec3.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 14:07:07
 */


#include "ch_dns_rdata_nsec3.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_nsec3_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_nsec3_t *nsec3 = (ch_dns_rdata_nsec3_t*)rdata;

	fprintf(fp,"Dump The rdata.nsec3:\n");
	fprintf(fp,"rdata.nsec3.hash_alg:%u\n",nsec3->hash_alg);
	fprintf(fp,"rdata.nsec3.flags:%u\n",nsec3->flags);
	fprintf(fp,"rdata.nsec3.iterations:%u\n",nsec3->iterations);

	if(nsec3->salt_len){
	
		ch_hex_string_dump("rdata.nsec3.salt",nsec3->salt,nsec3->salt_len,fp);
	}else{
	
		fprintf(fp,"rdata.nsec3.salt:none\n");
	}

	ch_base64_string_dump("rdata.nsec3.next",nsec3->next,nsec3->hash_len,fp);
	ch_hex_string_dump("rdata.nsec3.typebitmap",nsec3->typebitmap,nsec3->typebitmap_len,fp);

}

static ssize_t  _rdata_nsec3_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_nsec3_t *nsec3 = (ch_dns_rdata_nsec3_t*)rdata;
	ssize_t rc,len = 0;


	CH_DOUT_UINT8_WRITE(dout,nsec3->hash_alg,len,rc);
	CH_DOUT_UINT8_WRITE(dout,nsec3->flags,len,rc);
	CH_DOUT_UINT16_WRITE(dout,nsec3->iterations,len,rc);

	CH_DOUT_STRING16_WRITE(dout,nsec3->salt,nsec3->salt_len,len,rc);
	CH_DOUT_STRING16_WRITE(dout,nsec3->next,nsec3->hash_len,len,rc);
	CH_DOUT_STRING16_WRITE(dout,nsec3->typebitmap,nsec3->typebitmap_len,len,rc);

	return len;
}

static void _rdata_nsec3_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

    ch_dns_rdata_nsec3_t *nsec3 = (ch_dns_rdata_nsec3_t*)rdata;

    ch_msgpack_store_map_start(dstore,"nsec3",6);
    ch_msgpack_store_write_uint8(dstore,"alg",nsec3->hash_alg);
    ch_msgpack_store_write_uint8(dstore,"flags",nsec3->flags);
    ch_msgpack_store_write_uint16(dstore,"iterations",nsec3->iterations);

    ch_msgpack_store_write_bin_kv(dstore,"salt",(void*)nsec3->salt,nsec3->salt_len);
    ch_msgpack_store_write_bin_kv(dstore,"next",(void*)nsec3->next,nsec3->hash_len);
    ch_msgpack_store_write_bin_kv(dstore,"tbtm",(void*)nsec3->typebitmap,nsec3->typebitmap_len);

}

static ch_dns_rdata_t * _rdata_nsec3_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_nsec3_t *nsec3 = (ch_dns_rdata_nsec3_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_nsec3_t));

	nsec3->rdata.rdata_dump = _rdata_nsec3_dump;
	nsec3->rdata.rdata_write = _rdata_nsec3_write;
	nsec3->rdata.rdata_store = _rdata_nsec3_store;

	nsec3->hash_alg = 0;
	nsec3->flags = 0;
	nsec3->iterations = 0;
	
	nsec3->salt_len = 0;
	nsec3->salt = NULL;

	nsec3->hash_len = 0;
	nsec3->next = NULL;

	nsec3->typebitmap_len =  0;
	nsec3->typebitmap = NULL;

	return (ch_dns_rdata_t*)nsec3;

}

static int _rdata_nsec3_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_nsec3_t *nsec3 = (ch_dns_rdata_nsec3_t*)rdata;
	
	if(rdata->dlen <6 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	nsec3->hash_alg = ch_dns_data_input_uint8_read(din);
	nsec3->flags = ch_dns_data_input_uint8_read(din);
	nsec3->iterations = ch_dns_data_input_uint16_read(din);
	
	nsec3->salt_len = ch_dns_data_input_uint8_read(din);

	CH_DNS_DLEN_CHECK((uint16_t)nsec3->salt_len,din,-1);
	if(nsec3->salt_len){
	
		nsec3->salt = ch_dns_data_input_bytes_read(din,mp,nsec3->salt_len);
	}

	nsec3->hash_len = ch_dns_data_input_uint8_read(din);
	CH_DNS_DLEN_CHECK((uint16_t)nsec3->hash_len,din,-1);
	nsec3->next = ch_dns_data_input_bytes_read(din,mp,nsec3->hash_len);

	nsec3->typebitmap_len = ch_dns_data_input_rdlen(din);

	CH_DNS_DLEN_CHECK(nsec3->typebitmap_len,din,-1);
	nsec3->typebitmap = ch_dns_data_input_bytes_read(din,mp,nsec3->typebitmap_len);

	return 0;
}


static ch_dns_rdata_parser_t nsec3_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_nsec3,
	_rdata_nsec3_create,
	_rdata_nsec3_parse
};


void ch_dns_rdata_nsec3_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&nsec3_parser);
}


