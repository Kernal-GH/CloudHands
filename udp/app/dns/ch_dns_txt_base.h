/*
 *
 *      Filename: ch_dns_txt_base.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-15 10:05:40
 * Last Modified: 2018-06-15 10:05:40
 */

#ifndef CH_DNS_TXT_BASE_H
#define CH_DNS_TXT_BASE_H

typedef struct ch_dns_txt_base_t ch_dns_txt_base_t;

#include "ch_tables.h"
#include "ch_dns_data_input.h"
#include "ch_data_output.h"
#include "ch_msgpack_store.h"

struct ch_dns_txt_base_t {

	ch_array_header_t *txts;

};

#define ch_dns_txt_base_add(txt_base,text) *(unsigned char**)ch_array_push(txt_base->txts)=text

extern void ch_dns_txt_base_init(ch_pool_t *mp,ch_dns_txt_base_t *txt_base);

extern int ch_dns_txt_base_parse(ch_pool_t *mp,ch_dns_txt_base_t *txt_base,ch_dns_data_input_t *din);


extern void ch_dns_txt_base_dump(ch_dns_txt_base_t *txt_base,FILE *fp);


extern ssize_t ch_dns_txt_base_write(ch_dns_txt_base_t *txt_base,ch_data_output_t *dout);

extern void ch_dns_txt_base_store(ch_dns_txt_base_t *txt_base,ch_msgpack_store_t *dstore);


#define CH_DNS_TXT_BASE_WRITE(dout,txt_base,len,rc) do {   \
	if(-1 == (rc = ch_dns_txt_base_write(txt_base,dout)))  \
		return -1;										   \
	len+=rc;										       \
}while(0)

#endif /*CH_DNS_TXT_BASE_H*/
