/*
 *
 *      Filename: ch_dns_sec_sig.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-13 10:05:45
 * Last Modified: 2018-06-13 10:05:45
 */

#ifndef CH_DNS_SEC_SIG
#define CH_DNS_SEC_SIG

typedef struct ch_dns_sec_sig_t ch_dns_sec_sig_t;

#include "ch_dns_data_input.h"
#include "ch_data_output.h"
#include "ch_mpool.h"
#include "ch_dns_name.h"

struct ch_dns_sec_sig_t {

	uint16_t covered;
	uint8_t alg;
	uint8_t labels;
	uint32_t origttl;
	uint32_t expire;
	uint32_t timeSigned;
	uint16_t footprint;
	ch_dns_name_t signer;

	uint16_t sign_len;
	unsigned char *signature;

};

extern void ch_dns_sec_sig_init(ch_dns_sec_sig_t *ssig);

extern int ch_dns_sec_sig_parse(ch_pool_t *mp,ch_dns_sec_sig_t *ssig,ch_dns_data_input_t *din);


extern void ch_dns_sec_sig_dump(ch_dns_sec_sig_t *ssig,FILE *fp);


extern ssize_t ch_dns_sec_sig_write(ch_dns_sec_sig_t *ssig,ch_data_output_t *dout);

extern void ch_dns_sec_sig_store(ch_dns_sec_sig_t *ssig,ch_msgpack_store_t *dstore);


#define CH_DNS_SEC_SIG_WRITE(dout,ssig,len,rc) do {   \
	if(-1 == (rc = ch_dns_sec_sig_write(ssig,dout)))  \
		return -1;									  \
	len+=rc;										  \
}while(0)

#endif /*CH_DNS_SEC_SIG*/
