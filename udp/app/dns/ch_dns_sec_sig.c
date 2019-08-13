/*
 *
 *      Filename: ch_dns_sec_sig.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-13 10:15:48
 * Last Modified: 2018-06-14 19:03:00
 */

#include "ch_dns_sec_sig.h"
#include "ch_dns_rdata.h"
#include "ch_util.h"

void ch_dns_sec_sig_init(ch_dns_sec_sig_t *ssig){

	ssig->covered = 0;
	ssig->alg = 0;
	ssig->labels = 0;
	ssig->origttl = 0;
	ssig->expire = 0;
	ssig->timeSigned = 0;
	ssig->footprint = 0;

	CH_DNS_NAME_INIT(&ssig->signer);

	ssig->sign_len = 0;
	ssig->signature = NULL;
}


int ch_dns_sec_sig_parse(ch_pool_t *mp,ch_dns_sec_sig_t *ssig,ch_dns_data_input_t *din){

	size_t rlen = ch_dns_data_input_rdlen(din);
	if(rlen<18)
		return -1;

	ssig->covered = ch_dns_data_input_uint16_read(din);
	ssig->alg = ch_dns_data_input_uint8_read(din);
	ssig->labels = ch_dns_data_input_uint8_read(din);
	ssig->origttl = ch_dns_data_input_uint32_read(din);
	ssig->expire = ch_dns_data_input_uint32_read(din);
	ssig->timeSigned = ch_dns_data_input_uint32_read(din);
	ssig->footprint = ch_dns_data_input_uint16_read(din);

	if(-1 == ch_dns_name_parse(mp,din,&ssig->signer))
		return -1;

	ssig->sign_len = ch_dns_data_input_rdlen(din);
	ssig->signature = ch_dns_data_input_bytes_read(din,mp,ssig->sign_len);

	return 0;
}

void ch_dns_sec_sig_dump(ch_dns_sec_sig_t *ssig,FILE *fp){

	fprintf(fp,"dns.sec.sig.covered:%u\n",ssig->covered);
	fprintf(fp,"dns.sec.sig.alg:%u\n",ssig->alg);
	fprintf(fp,"dns.sec.sig.labels:%u\n",ssig->labels);
	fprintf(fp,"dns.sec.sig.origttl:%lu\n",(unsigned long)ssig->origttl);
	fprintf(fp,"dns.sec.sig.expire:%lu\n",(unsigned long)ssig->expire);
	fprintf(fp,"dns.sec.sig.timeSigned:%lu\n",(unsigned long)ssig->timeSigned);
	fprintf(fp,"dns.sec.sig.footprint:%u\n",ssig->footprint);
	fprintf(fp,"dns.sec.sig.labels:%u\n",ssig->labels);

	fprintf(fp,"dns.sec.sig.signer:\n");
	ch_dns_name_dump(&ssig->signer,fp);

	ch_base64_string_dump("dns.sec.sig.signature",ssig->signature,ssig->sign_len,fp);

}

ssize_t ch_dns_sec_sig_write(ch_dns_sec_sig_t *ssig,ch_data_output_t *dout){

	ssize_t rc,len = 0;
	
	size_t sign_len = ssig->sign_len;
	unsigned char *signature = ssig->signature;

	CH_DOUT_UINT16_WRITE(dout,ssig->covered,len,rc);
	CH_DOUT_UINT8_WRITE(dout,ssig->alg,len,rc);
	CH_DOUT_UINT8_WRITE(dout,ssig->labels,len,rc);
	CH_DOUT_UINT32_WRITE(dout,ssig->origttl,len,rc);
	CH_DOUT_UINT32_WRITE(dout,ssig->expire,len,rc);
	CH_DOUT_UINT32_WRITE(dout,ssig->timeSigned,len,rc);
	CH_DOUT_UINT16_WRITE(dout,ssig->footprint,len,rc);

	CH_DNS_NAME_WRITE(dout,&ssig->signer,len,rc);

	CH_DOUT_STRING16_WRITE(dout,signature,sign_len,len,rc);


	return len;
}

void ch_dns_sec_sig_store(ch_dns_sec_sig_t *ssig,ch_msgpack_store_t *dstore){

    size_t sign_len = ssig->sign_len;   
    const char *signature = (const char*)ssig->signature;

    ch_msgpack_store_map_start(dstore,"ssig",9);

    ch_msgpack_store_write_uint16(dstore,"covered",ssig->covered);
    ch_msgpack_store_write_uint8(dstore,"alg",ssig->alg);
    ch_msgpack_store_write_uint8(dstore,"labels",ssig->labels);
    ch_msgpack_store_write_uint32(dstore,"origttl",ssig->origttl);
    ch_msgpack_store_write_uint32(dstore,"expire",ssig->expire);
    ch_msgpack_store_write_uint32(dstore,"timeSigned",ssig->timeSigned);
    ch_msgpack_store_write_uint16(dstore,"footprint",ssig->footprint);

    ch_dns_name_store(&ssig->signer,dstore);
    ch_msgpack_store_write_bin_kv(dstore,"signature",(void*)signature,sign_len);

}

