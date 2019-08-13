/*
 *
 *      Filename: ch_dns_sec_key.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-13 10:15:48
 * Last Modified: 2018-06-14 19:06:46
 */

#include "ch_dns_sec_key.h"
#include "ch_dns_rdata.h"

void ch_dns_sec_key_init(ch_dns_sec_key_t *skey){


	skey->flags = 0;
	skey->proto = 0;
	skey->alg = 0;
	skey->public_key_len = 0;
	skey->public_key = NULL;
}


int ch_dns_sec_key_parse(ch_pool_t *mp,ch_dns_sec_key_t *skey,ch_dns_data_input_t *din){

	size_t rlen = ch_dns_data_input_rdlen(din);
	if(rlen<4)
		return -1;

	skey->flags = ch_dns_data_input_uint16_read(din);
	skey->proto = ch_dns_data_input_uint8_read(din);
	skey->alg = ch_dns_data_input_uint8_read(din);

	skey->public_key_len = ch_dns_data_input_rdlen(din);
	skey->public_key = ch_dns_data_input_bytes_read(din,mp,skey->public_key_len);

	return 0;
}

void ch_dns_sec_key_dump(ch_dns_sec_key_t *skey,FILE *fp){

	fprintf(fp,"dns.sec.key.flags:%u\n",skey->flags);
	fprintf(fp,"dns.sec.key.proto:%u\n",skey->proto);
	fprintf(fp,"dns.sec.key.alg:%u\n",skey->alg);

	ch_base64_string_dump("dns.sec.key.public_key",skey->public_key,skey->public_key_len,fp);


}

void ch_dns_sec_key_store(ch_dns_sec_key_t *skey,ch_msgpack_store_t *dstore) {

    ch_msgpack_store_map_start(dstore,"skey",4);
    ch_msgpack_store_write_uint16(dstore,"flags",skey->flags);
    ch_msgpack_store_write_uint8(dstore,"proto",skey->proto);
    ch_msgpack_store_write_uint8(dstore,"alg",skey->alg);
    ch_msgpack_store_write_bin_kv(dstore,"pubKey",(void*)skey->public_key,skey->public_key_len);

}

ssize_t ch_dns_sec_key_write(ch_dns_sec_key_t *skey,ch_data_output_t *dout){

	ssize_t rc,len = 0;
	size_t dlen = skey->public_key_len;
	unsigned char *data = skey->public_key;

	CH_DOUT_UINT16_WRITE(dout,skey->flags,len,rc);
	CH_DOUT_UINT8_WRITE(dout,skey->proto,len,rc);
	CH_DOUT_UINT8_WRITE(dout,skey->alg,len,rc);

	CH_DOUT_STRING16_WRITE(dout,data,dlen,len,rc);


	return len;
}


