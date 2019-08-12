/*
 *
 *      Filename: ch_dns_sec_key.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-13 10:05:45
 * Last Modified: 2018-06-13 10:05:45
 */

#ifndef CH_DNS_SEC_KEY
#define CH_DNS_SEC_KEY

typedef struct ch_dns_sec_key_t ch_dns_sec_key_t;

#include "ch_dns_data_input.h"
#include "ch_data_output.h"
#include "ch_mpool.h"
#include "ch_msgpack_store.h"

struct ch_dns_sec_key_t {

	uint16_t flags;
	uint8_t proto;
	uint8_t alg;
	uint16_t public_key_len;

	unsigned char *public_key;


};

extern void ch_dns_sec_key_init(ch_dns_sec_key_t *skey);

extern int ch_dns_sec_key_parse(ch_pool_t *mp,ch_dns_sec_key_t *skey,ch_dns_data_input_t *din);


extern void ch_dns_sec_key_dump(ch_dns_sec_key_t *skey,FILE *fp);


extern ssize_t ch_dns_sec_key_write(ch_dns_sec_key_t *skey,ch_data_output_t *dout);

extern void ch_dns_sec_key_store(ch_dns_sec_key_t *skey,ch_msgpack_store_t *dstore);


#define CH_DNS_SEC_KEY_WRITE(dout,skey,len,rc) do {   \
	if(-1 == (rc = ch_dns_sec_key_write(skey,dout)))  \
		return -1;									  \
	len+=rc;										  \
}while(0)

#endif /*CH_DNS_SEC_KEY*/
