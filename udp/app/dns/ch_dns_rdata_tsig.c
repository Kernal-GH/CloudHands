/*
 *
 *      Filename: ch_dns_rdata_tsig.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 14:03:16
 */


#include "ch_dns_rdata_tsig.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_tsig_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_tsig_t *tsig = (ch_dns_rdata_tsig_t*)rdata;


	fprintf(fp,"Dump The rdata.tsig:\n");
	
	fprintf(fp,"rdata.tsig.alg:\n");
	ch_dns_name_dump(&tsig->alg,fp);

	fprintf(fp,"rdata.tsig.timeSigned:%lu\n",(unsigned long)tsig->timeSigned);
	fprintf(fp,"rdata.tsig.fudge:%u\n",tsig->fudge);

	fprintf(fp,"rdata.tsig.origin_Id:%u\n",tsig->orig_id);
	fprintf(fp,"rdata.tsig.error:%u\n",tsig->error);

	ch_base64_string_dump("rdata.tsig.signature",tsig->signature,tsig->sig_len,fp);
	ch_base64_string_dump("rdata.tsig.other",tsig->other,tsig->other_len,fp);

}

static ssize_t  _rdata_tsig_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_tsig_t *tsig = (ch_dns_rdata_tsig_t*)rdata;
	ssize_t rc,len = 0;

	CH_DNS_NAME_WRITE(dout,&tsig->alg,len,rc);

	CH_DOUT_UINT64_WRITE(dout,tsig->timeSigned,len,rc);
	CH_DOUT_UINT16_WRITE(dout,tsig->fudge,len,rc);


	CH_DOUT_STRING16_WRITE(dout,tsig->signature,tsig->sig_len,len,rc);
	
	CH_DOUT_UINT16_WRITE(dout,tsig->orig_id,len,rc);
	CH_DOUT_UINT16_WRITE(dout,tsig->error,len,rc);
	
	CH_DOUT_STRING16_WRITE(dout,tsig->other,tsig->other_len,len,rc);

	return len;
}

static void _rdata_tsig_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

      ch_dns_rdata_tsig_t *tsig = (ch_dns_rdata_tsig_t*)rdata;  
      ch_msgpack_store_map_start(dstore,"tsig",7);
      ch_dns_name_store(&tsig->alg,dstore);
      ch_msgpack_store_write_uint64(dstore,"timeSigned",tsig->timeSigned);
      ch_msgpack_store_write_uint16(dstore,"fudge",tsig->fudge);
      ch_msgpack_store_write_bin_kv(dstore,"signature",(void*)tsig->signature,tsig->sig_len);
      ch_msgpack_store_write_uint16(dstore,"origID",tsig->orig_id);
      ch_msgpack_store_write_uint16(dstore,"error",tsig->error);
      ch_msgpack_store_write_bin_kv(dstore,"other",(void*)tsig->other,tsig->other_len);

}

static ch_dns_rdata_t * _rdata_tsig_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_tsig_t *tsig = (ch_dns_rdata_tsig_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_tsig_t));

	tsig->rdata.rdata_dump = _rdata_tsig_dump;
	tsig->rdata.rdata_write = _rdata_tsig_write;
	tsig->rdata.rdata_store = _rdata_tsig_store;

	CH_DNS_NAME_INIT(&tsig->alg);
	tsig->timeSigned = 0;
	tsig->fudge = 0;
	tsig->sig_len = 0;
	tsig->signature = NULL;

	tsig->orig_id = 0;
	tsig->error = 0;

	tsig->other_len = 0;
	tsig->other = NULL;

	return (ch_dns_rdata_t*)tsig;

}

static int _rdata_tsig_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	uint64_t time_high,time_low;

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_tsig_t *tsig = (ch_dns_rdata_tsig_t*)rdata;
	
	if(rdata->dlen <16 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	CH_DNS_NAME_PARSE(mp,din,&tsig->alg);

	time_high = ch_dns_data_input_uint16_read(din);
	time_low =  ch_dns_data_input_uint32_read(din);

	tsig->timeSigned = (time_high<<32)+time_low;

	tsig->fudge = ch_dns_data_input_uint16_read(din);

	tsig->sig_len = ch_dns_data_input_uint16_read(din);
	CH_DNS_DLEN_CHECK(tsig->sig_len,din,-1);
	
	if(tsig->sig_len){
	
		tsig->signature = ch_dns_data_input_bytes_read(din,mp,tsig->sig_len);
	}
	
	tsig->orig_id = ch_dns_data_input_uint16_read(din);
	tsig->error = ch_dns_data_input_uint16_read(din);

	tsig->other_len = ch_dns_data_input_uint16_read(din);
	CH_DNS_DLEN_CHECK(tsig->other_len,din,-1);
	if(tsig->other_len){
	
		tsig->other = ch_dns_data_input_bytes_read(din,mp,tsig->other_len);
	}

	return 0;
}


static ch_dns_rdata_parser_t tsig_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_tsig,
	_rdata_tsig_create,
	_rdata_tsig_parse
};


void ch_dns_rdata_tsig_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&tsig_parser);
}


