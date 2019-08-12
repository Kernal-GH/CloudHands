/*
 *
 *      Filename: ch_dns_name.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-26 18:13:04
 * Last Modified: 2018-04-26 18:13:04
 */

#ifndef CH_DNS_NAME_H
#define CH_DNS_NAME_H

typedef struct ch_dns_name_t ch_dns_name_t;

#include "ch_dns_data_input.h"
#include "ch_mpool.h"
#include "ch_data_output.h"
#include "ch_msgpack_store.h"

/*%
 * Max size of a  domain name
 */
#define DNS_NAME_MAXLEN 255

struct ch_dns_name_t {

	unsigned char *ndata;

	uint16_t length;

	uint16_t labels;

};

#define CH_DNS_NAME_INIT(name) do {(name)->ndata = NULL;(name)->length = 0;(name)->labels = 0;}while(0)

extern int ch_dns_name_parse(ch_pool_t *mp,ch_dns_data_input_t *din,ch_dns_name_t *dname);

#define CH_DNS_NAME_PARSE(mp,din,name) do {  \
	if(-1 == ch_dns_name_parse(mp,din,name)) \
		return -1;							 \
}while(0)

static inline void ch_dns_name_clone(ch_pool_t *mp,ch_dns_name_t *dst,ch_dns_name_t *src){


	dst->length = src->length;
	dst->labels = src->labels;
	dst->ndata = (unsigned char*)ch_pstrndup(mp,(const char*)src->ndata,src->length);
}

static inline void ch_dns_name_store(ch_dns_name_t *name,ch_msgpack_store_t *dstore){

    ch_msgpack_store_map_start(dstore,"name",2);
    ch_msgpack_store_write_uint16(dstore,"labels",name->labels);
    ch_msgpack_store_write_str_wlen(dstore,"name",(const char*)name->ndata,name->length);

}

static inline void ch_dns_name_store_wkey(ch_dns_name_t *name,ch_msgpack_store_t *dstore,const char *k){

    ch_msgpack_store_map_start(dstore,k,2);
    ch_msgpack_store_write_uint16(dstore,"labels",name->labels);
    ch_msgpack_store_write_str_wlen(dstore,"name",(const char*)name->ndata,name->length);

}

static inline ssize_t ch_dns_name_write(ch_data_output_t *dout,ch_dns_name_t *name){

	ssize_t rc,len = 0;


	CH_DOUT_UINT16_WRITE(dout,name->labels,len,rc);
	CH_DOUT_STRING16_WRITE(dout,name->ndata,name->length,len,rc);

	return len;
}
#define CH_DNS_NAME_WRITE(dout,name,len,rc) do {  \
	if(-1 == (rc = ch_dns_name_write(dout,name))) \
		return -1;                                \
	len+=rc;                                      \
}while(0)

static inline void ch_dns_name_dump(ch_dns_name_t *name,FILE *fp){

	fprintf(fp,"name.ndata:%s\n",(const char*)name->ndata);
	fprintf(fp,"name.labels:%u\n",name->labels);
	fprintf(fp,"name.length:%u\n",name->length);
}

#endif /*CH_DNS_NAME_H*/
