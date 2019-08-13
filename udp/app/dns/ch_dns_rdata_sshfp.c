/*
 *
 *      Filename: ch_dns_rdata_sshfp.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-12 16:31:13
 * Last Modified: 2018-06-26 14:00:49
 */


#include "ch_dns_rdata_sshfp.h"
#include "ch_log.h"
#include "ch_util.h"

static void _rdata_sshfp_dump(ch_dns_rdata_t *rdata,FILE *fp,void *priv_data ch_unused){

	ch_dns_rdata_sshfp_t *sshfp = (ch_dns_rdata_sshfp_t*)rdata;

	fprintf(fp,"Dump The rdata.sshfp:\n");
	fprintf(fp,"rdata.sshfp.alg:%u\n",sshfp->alg);
	fprintf(fp,"rdata.sshfp.digest_type:%u\n",sshfp->digest_type);

	ch_hex_string_dump("rdata.sshfp.fingerprint",sshfp->fingerprint,sshfp->fp_len,fp);

}

static ssize_t  _rdata_sshfp_write(ch_dns_rdata_t *rdata,ch_data_output_t *dout,void *priv_data ch_unused){

	ch_dns_rdata_sshfp_t *sshfp = (ch_dns_rdata_sshfp_t*)rdata;
	ssize_t rc,len = 0;

	unsigned char *fingerprint = sshfp->fingerprint;
	uint16_t fp_len = sshfp->fp_len;

	CH_DOUT_UINT8_WRITE(dout,sshfp->alg,len,rc);
	CH_DOUT_UINT8_WRITE(dout,sshfp->digest_type,len,rc);

	CH_DOUT_STRING16_WRITE(dout,fingerprint,fp_len,len,rc);

	return len;
}

static void _rdata_sshfp_store(ch_dns_rdata_t *rdata,ch_msgpack_store_t *dstore){

     ch_dns_rdata_sshfp_t *sshfp = (ch_dns_rdata_sshfp_t*)rdata; 
     const char *fingerprint = (const char*)sshfp->fingerprint;

     ch_msgpack_store_map_start(dstore,"sshfp",3);
     ch_msgpack_store_write_uint8(dstore,"alg",sshfp->alg);
     ch_msgpack_store_write_uint8(dstore,"digestType",sshfp->digest_type);
     ch_msgpack_store_write_bin_kv(dstore,"fingerprint",(void*)fingerprint,sshfp->fp_len);

}

static ch_dns_rdata_t * _rdata_sshfp_create(ch_pool_t *mp,void *priv_data ch_unused){


	ch_dns_rdata_sshfp_t *sshfp = (ch_dns_rdata_sshfp_t*)ch_pcalloc(mp,sizeof(ch_dns_rdata_sshfp_t));

	sshfp->rdata.rdata_dump = _rdata_sshfp_dump;
	sshfp->rdata.rdata_write = _rdata_sshfp_write;
	sshfp->rdata.rdata_store = _rdata_sshfp_store;

	sshfp->alg = 0;
	sshfp->digest_type = 0;
	sshfp->fp_len = 0;
	sshfp->fingerprint = NULL;

	return (ch_dns_rdata_t*)sshfp;

}

static int _rdata_sshfp_parse(ch_pool_t *mp,ch_dns_rdata_t *rdata,void *priv_data ch_unused){

	ch_dns_data_input_t tmp,*din=&tmp;

	ch_dns_rdata_sshfp_t *sshfp = (ch_dns_rdata_sshfp_t*)rdata;
	
	if(rdata->dlen <2 || rdata->data == NULL)
		return -1;

	ch_dns_rdata_input_init(din,rdata);

	sshfp->alg = ch_dns_data_input_uint8_read(din);
	sshfp->digest_type = ch_dns_data_input_uint8_read(din);

	sshfp->fp_len = ch_dns_data_input_rdlen(din);
	CH_DNS_DLEN_CHECK(sshfp->fp_len,din,-1);

	sshfp->fingerprint = ch_dns_data_input_rbytes_read(din,mp);

	return 0;
}


static ch_dns_rdata_parser_t sshfp_parser = {

	{NULL,NULL},
	dns_rdataclass_in,
	dns_rdatatype_sshfp,
	_rdata_sshfp_create,
	_rdata_sshfp_parse
};


void ch_dns_rdata_sshfp_init(ch_dns_rdata_pool_t *rdata_pool){

	ch_dns_rdata_parser_register(rdata_pool,&sshfp_parser);
}


