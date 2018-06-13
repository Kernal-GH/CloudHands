/*
 *
 *      Filename: ch_data_output.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-08 13:31:23
 * Last Modified: 2018-05-08 13:31:23
 */

#ifndef CH_DATA_OUTPUT_H
#define CH_DATA_OUTPUT_H

typedef struct ch_data_output_t ch_data_output_t;

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define DOUT_SIZE_DEFAULT 64*1024
#define DOUT_SIZE_EXT 32

struct ch_data_output_t {

	void *base;
	void *pos;
	void *end;

};

#define CH_DOUT_CONTENT(dout) ((dout)->base)
#define CH_DOUT_FULL(dout,sz) ((dout)->pos+sz>(dout)->end)
#define CH_DOUT_SIZE(dout) ((dout)->end-(dout)->base)
#define CH_DOUT_CONTENT_SIZE(dout) ((dout)->pos-(dout)->base)
#define CH_DOUT_POS_UPDATE(dout,sz) ((dout)->pos = (dout)->pos+(sz))
#define CH_DOUT_RESET(dout) ((dout)->pos = (dout)->base)

static inline int ch_dout_init(ch_data_output_t *dout){


	dout->base = malloc(DOUT_SIZE_DEFAULT+DOUT_SIZE_EXT);
	if(dout->base == NULL)
		return -1;

	dout->pos = dout->base;
	dout->end = dout->base+DOUT_SIZE_DEFAULT;

	return 0;
}

static inline int ch_dout_incr(ch_data_output_t *dout,size_t incr_size){

	size_t nsize = CH_DOUT_SIZE(dout)+incr_size;
	size_t osize = CH_DOUT_CONTENT_SIZE(dout);

	void *addr = malloc(nsize+DOUT_SIZE_EXT);
	if(addr == NULL)
		return -1;

	/*copy old data*/
	if(dout->base){

		if(osize>0)
			memcpy(addr,dout->base,osize);
		
		free(dout->base);
	}

	dout->base = addr;
	dout->pos = dout->base+osize;
	dout->end = dout->base+nsize;

	return 0;
}

#define DOUT_VALUE_WRITE(buf,value,byte_num) 		\
	do{												\
		size_t bytes,i;								\
		bytes = (byte_num)*8;						\
		for(i=1;i<=(byte_num);i++)					\
		{											\
			buf[i-1]=((value)>>(bytes-i*8))&0xff;	\
		}											\
	}while(0)

#define DOUT_CHECK(dout,sz) do{		\
	if(CH_DOUT_FULL(dout,sz))		\
	{								\
		if(ch_dout_incr(dout,4*sz))	\
			return -1;				\
	}								\
}while(0)

/*write basic data type functions*/

static inline void dout_write(ch_data_output_t *dout,unsigned char *bytes,size_t len){

    memcpy(dout->pos,(void*)bytes,len);
    CH_DOUT_POS_UPDATE(dout,len);
}

static inline ssize_t ch_dout_bool_write(ch_data_output_t *dout,int v){

    unsigned char barr[1] = {0};
    DOUT_CHECK(dout,1);
    DOUT_VALUE_WRITE(barr,v,1);
    dout_write(dout,barr,1);
    return 1;
}

static inline ssize_t ch_dout_short_write(ch_data_output_t *dout,short v){

    size_t sz = sizeof(short);
    unsigned char barr[sizeof(short)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_int_write(ch_data_output_t *dout,int v){

    size_t sz = sizeof(int);
    unsigned char barr[sizeof(int)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_long_write(ch_data_output_t *dout,long v){

    size_t sz = sizeof(long);
    unsigned char barr[sizeof(long)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_ushort_write(ch_data_output_t *dout,unsigned short v){

    size_t sz = sizeof(unsigned short);
    unsigned char barr[sizeof(unsigned short)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_uint_write(ch_data_output_t *dout,unsigned int v){

    size_t sz = sizeof(unsigned int);
    unsigned char barr[sizeof(unsigned int)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_ulong_write(ch_data_output_t *dout,unsigned long v){

    size_t sz = sizeof(unsigned long);
    unsigned char barr[sizeof(unsigned long)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_size_write(ch_data_output_t *dout,size_t v){

    size_t sz = sizeof(size_t);
    unsigned char barr[sizeof(size_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_ssize_write(ch_data_output_t *dout,ssize_t v){

    size_t sz = sizeof(ssize_t);
    unsigned char barr[sizeof(ssize_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_off_write(ch_data_output_t *dout,off_t v){

    size_t sz = sizeof(off_t);
    unsigned char barr[sizeof(off_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_int8_write(ch_data_output_t *dout,int8_t v){

    size_t sz = sizeof(int8_t);
    unsigned char barr[sizeof(int8_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_int16_write(ch_data_output_t *dout,int16_t v){

    size_t sz = sizeof(int16_t);
    unsigned char barr[sizeof(int16_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_int32_write(ch_data_output_t *dout,int32_t v){

    size_t sz = sizeof(int32_t);
    unsigned char barr[sizeof(int32_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_int64_write(ch_data_output_t *dout,int64_t v){

    size_t sz = sizeof(int64_t);
    unsigned char barr[sizeof(int64_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_dout_uint8_write(ch_data_output_t *dout,uint8_t v){

    size_t sz = sizeof(uint8_t);
    unsigned char barr[sizeof(uint8_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}
#define CH_DOUT_UINT8_WRITE(dout,v,len,rc) do {			\
	if(-1 == (rc = ch_dout_uint8_write(dout,v)))			\
		return -1;											\
	len+=rc;												\
}while(0) 

static inline ssize_t ch_dout_uint16_write(ch_data_output_t *dout,uint16_t v){

    size_t sz = sizeof(uint16_t);
    unsigned char barr[sizeof(uint16_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}
#define CH_DOUT_UINT16_WRITE(dout,v,len,rc) do {			\
	if(-1 == (rc = ch_dout_uint16_write(dout,v)))			\
		return -1;											\
	len+=rc;												\
}while(0) 

static inline ssize_t ch_dout_uint32_write(ch_data_output_t *dout,uint32_t v){

    size_t sz = sizeof(uint32_t);
    unsigned char barr[sizeof(uint32_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}
#define CH_DOUT_UINT32_WRITE(dout,v,len,rc) do {			\
	if(-1 == (rc = ch_dout_uint32_write(dout,v)))			\
		return -1;											\
	len+=rc;												\
}while(0) 

static inline ssize_t ch_dout_uint64_write(ch_data_output_t *dout,uint64_t v){

    size_t sz = sizeof(uint64_t);
    unsigned char barr[sizeof(uint64_t)] = {0};
    DOUT_CHECK(dout,sz);
    DOUT_VALUE_WRITE(barr,v,sz);
    dout_write(dout,barr,sz);
    return (ssize_t)sz;
}
#define CH_DOUT_UINT64_WRITE(dout,v,len,rc) do {			\
	if(-1 == (rc = ch_dout_uint64_write(dout,v)))			\
		return -1;											\
	len+=rc;												\
}while(0) 

static inline ssize_t ch_dout_bytes_write(ch_data_output_t *dout,unsigned char* bytes,size_t len){

    size_t rlen = len+sizeof(size_t);
    DOUT_CHECK(dout,rlen);
    ch_dout_size_write(dout,len);
    dout_write(dout,bytes,len);

    return (ssize_t)rlen;
}
#define CH_DOUT_BYTES_WRITE(dout,bytes,blen,len,rc) do {	\
	if(-1 == (rc = ch_dout_bytes_write(dout,bytes,slen)))	\
		return -1;											\
	len+=rc;												\
}while(0) 

static inline ssize_t ch_dout_string_write(ch_data_output_t *dout,unsigned char* str){

    size_t len = strlen((const char*)str);
    size_t rlen = len+sizeof(size_t);
    DOUT_CHECK(dout,rlen);
    ch_dout_size_write(dout,len);
    dout_write(dout,str,len);

    return (ssize_t)rlen;
}

#define CH_DOUT_STRING_WRITE(dout,str,len,rc) do {			\
	if(-1 == (rc = ch_dout_string_write(dout,str)))			\
		return -1;											\
	len+=rc;												\
}while(0) 

static inline ssize_t ch_dout_string16_write(ch_data_output_t *dout,unsigned char *str,uint16_t len){

	if(str == NULL)
		str = (unsigned char*)"";

	uint16_t rlen = len+sizeof(uint16_t);
    DOUT_CHECK(dout,rlen);
    ch_dout_uint16_write(dout,len);
    dout_write(dout,str,len);

    return (ssize_t)rlen;

}

#define CH_DOUT_STRING16_WRITE(dout,str,slen,len,rc) do {	\
	if(-1 == (rc = ch_dout_string16_write(dout,str,slen)))	\
		return -1;											\
	len+=rc;												\
}while(0) 


#define CH_DOUT_STRING16_WRITE2(dout,str,len,rc) do { \
	if(-1 == (rc = ch_dout_string16_write(dout,str?(unsigned char*)str:(unsigned char*)"",str?strlen((const char*)str):0))) \
		return -1; \
	len+=rc; \
}while(0)

#endif /*CH_DATA_OUTPUT_H*/
