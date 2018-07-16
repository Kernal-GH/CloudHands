/*
 *
 *      Filename: ch_dns_data_input.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-04 11:14:56
 * Last Modified: 2018-05-04 11:14:56
 */

#ifndef CH_DNS_DATA_INPUT_H
#define CH_DNS_DATA_INPUT_H

typedef struct ch_dns_data_input_t ch_dns_data_input_t;

#include <stdint.h>
#include <stdlib.h>
#include "ch_string.h"
#include "ch_log.h"

struct ch_dns_data_input_t {

	void *base;
	void *pos;
	void *last;
};

#define ch_dns_data_input_init(din,b,p,e) do { \
	din->base = b; \
	din->pos = p;  \
	din->last = e; \
}while(0)

#define ch_dns_data_input_pos_update(din,sz) (din->pos = din->pos+(sz))
#define ch_dns_data_input_read_over(din,sz) (din->pos+sz>din->last)
#define ch_dns_data_input_pos(din) (din->pos)
#define ch_dns_data_input_base(din) (din->base)
#define ch_dns_data_input_last(din) (din->last)
#define ch_dns_data_input_rdlen(din) (din->last-din->pos)
#define ch_dns_data_input_offset_pos(din) (din->pos-din->base)
#define ch_dns_data_input_offset_last(din) (din->last-din->base)
#define ch_dns_data_input_empty(din) (din->pos>=din->last)

/*read basic data type functions*/
#define VALUE_READ(buf, value, byte_num,type)		 	 \
do{														 \
	size_t bytes,i;										 \
    void *buf_p = buf;									 \
	unsigned char b;									 \
	bytes = (byte_num)*8;								 \
	for (i=1; i<=byte_num; i++)							 \
	{													 \
		b=*((unsigned char*)buf_p);						 \
		buf_p+=1;										 \
		(value) += ((type)(b&0xFF)<<(bytes-i*8));		 \
	}													 \
} while(0)

static inline uint8_t ch_dns_data_input_uint8_read(ch_dns_data_input_t *din){

    uint8_t vv = 0;
    size_t sz = sizeof(uint8_t);

    if(ch_dns_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->pos,vv,sz,uint8_t);
    ch_dns_data_input_pos_update(din,sz);
    return vv;
}

static inline uint16_t ch_dns_data_input_uint16_read(ch_dns_data_input_t *din){

    uint16_t vv = 0;
    size_t sz = sizeof(uint16_t);

    if(ch_dns_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->pos,vv,sz,uint16_t);
    ch_dns_data_input_pos_update(din,sz);
    return vv;
}

static inline uint32_t ch_dns_data_input_uint32_read(ch_dns_data_input_t *din){

    uint32_t vv = 0;
    size_t sz = sizeof(uint32_t);

    if(ch_dns_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->pos,vv,sz,uint32_t);
    ch_dns_data_input_pos_update(din,sz);
    return vv;
}

static inline void ch_dns_data_input_bstring_read(ch_dns_data_input_t *din,unsigned char** data,uint8_t *len){

    *data = NULL;
    *len = 0;

    if(ch_dns_data_input_read_over(din,sizeof(uint8_t)))
        return;

    *len = ch_dns_data_input_uint8_read(din);

    *data = din->pos;

    ch_dns_data_input_pos_update(din,*len);
}

static inline unsigned char * ch_dns_data_copy(ch_pool_t *mp,unsigned char* data,size_t dlen){


	unsigned char *result = ch_pcalloc(mp,dlen+1);
	if(result){
	
		memcpy((void*)result,(const void*)data,dlen);

	}

	return result;

}

static inline unsigned char* ch_dns_data_input_bstring_read_dup(ch_dns_data_input_t *din,ch_pool_t *mp){

	unsigned char *data;
	size_t dlen,rdlen;

    if(ch_dns_data_input_read_over(din,sizeof(uint8_t)))
        return NULL;

	rdlen = ch_dns_data_input_rdlen(din);
    dlen = ch_dns_data_input_uint8_read(din);

	if(rdlen<dlen)
		dlen = rdlen;

    data = (unsigned char*)din->pos;

    ch_dns_data_input_pos_update(din,dlen);

	return ch_dns_data_copy(mp,data,dlen);

}

static inline unsigned char * ch_dns_data_input_bytes_read(ch_dns_data_input_t *din,ch_pool_t *mp,size_t bytes)
{

	size_t r_bytes = bytes;
	size_t rdlen = ch_dns_data_input_rdlen(din);

	if(rdlen<bytes)
		r_bytes = rdlen;


    if(r_bytes == 0)
        return NULL;

    ch_dns_data_input_pos_update(din,r_bytes);
	
	return ch_dns_data_copy(mp,(unsigned char*)din->pos,r_bytes);

}

static inline unsigned char * ch_dns_data_input_rbytes_read(ch_dns_data_input_t *din,ch_pool_t *mp){

	unsigned char *data;
	size_t dlen;

	data = (unsigned char *)din->pos;
	dlen = ch_dns_data_input_rdlen(din);
	
	if(dlen == 0)
		return NULL;

	din->pos = din->last;

	return ch_dns_data_copy(mp,data,dlen);

}

#define CH_DNS_DLEN_CHECK(dlen,din,retv) do {											   \
	size_t vvv = dlen;                                                                       \
	if(vvv>512||vvv>(size_t)ch_dns_data_input_rdlen(din))											    \
	{																						\
		ch_log(CH_LOG_ERR,":%s,%d: too large dns data len:%d", __func__,__LINE__,(int)vvv);   \
		return (retv);																		\
	}																						\
}while(0)

#endif /*CH_DNS_DATA_INPUT_H*/
