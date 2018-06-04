/*************************************************************************
 > File Name: ch_bin_format.h
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月15日 星期三 10时25分29秒
 ************************************************************************/

#ifndef CH_BIN_FORMAT_H
#define CH_BIN_FORMAT_H
 
typedef struct ch_bin_format_t ch_bin_format_t;

#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct ch_bin_format_t {

    void *start;
    void *pos;
    void *end;

};

static inline size_t ch_bf_content_size(ch_bin_format_t *bf){

    return (size_t)(bf->pos - bf->start);
}

static inline int ch_bf_full(ch_bin_format_t *bf,size_t size){

    return bf->pos+size>bf->end;
}

static inline int ch_bf_empty(ch_bin_format_t *bf){

	return bf->pos == bf->start;
}

static inline void ch_bf_pos_update(ch_bin_format_t *bf,size_t size){
    bf->pos += size;
}

static inline void ch_bf_init(ch_bin_format_t *bf,void *start,void *end){

    bf->start = start;
    bf->end = end;
    bf->pos = bf->start;
}

static inline void ch_bf_init_with_len(ch_bin_format_t *bf,void *start,size_t len){

	bf->start = start;
	bf->pos = bf->start;
	bf->end = bf->start+len;

}

static inline void ch_bf_reset(ch_bin_format_t *bf){

    bf->pos = bf->start;
}

static inline void ch_bf_skip(ch_bin_format_t *bf,size_t size){

    if(ch_bf_full(bf,size)){
        bf->pos = bf->end;
    }else{
        bf->pos += size;
    }
}

#define DO_VALUE_WRITE(buf,value,byte_num) 			\
	do{							\
		size_t bytes,i;					\
		bytes = (byte_num)*8;				\
		for(i=1;i<=(byte_num);i++)			\
		{						\
			buf[i-1]=((value)>>(bytes-i*8))&0xff;	\
		}						\
	}while(0)

#define DO_CHECK(bf,sz) 				\
	do{							\
		if(!bf) 					\
			return -1;				\
		if(ch_bf_full(bf,sz)) 			\
			return -1;				\
	}while(0)

/*write basic data type functions*/

static inline void do_write(ch_bin_format_t *bf,unsigned char *bytes,size_t len){

    memcpy(bf->pos,(void*)bytes,len);
    ch_bf_pos_update(bf,len);
}

static inline ssize_t ch_bf_bool_write(ch_bin_format_t *bf,int v){

    unsigned char barr[1] = {0};
    DO_CHECK(bf,1);
    DO_VALUE_WRITE(barr,v,1);
    do_write(bf,barr,1);
    return 1;
}

static inline ssize_t ch_bf_short_write(ch_bin_format_t *bf,short v){

    size_t sz = sizeof(short);
    unsigned char barr[sizeof(short)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_int_write(ch_bin_format_t *bf,int v){

    size_t sz = sizeof(int);
    unsigned char barr[sizeof(int)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_long_write(ch_bin_format_t *bf,long v){

    size_t sz = sizeof(long);
    unsigned char barr[sizeof(long)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_ushort_write(ch_bin_format_t *bf,unsigned short v){

    size_t sz = sizeof(unsigned short);
    unsigned char barr[sizeof(unsigned short)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_uint_write(ch_bin_format_t *bf,unsigned int v){

    size_t sz = sizeof(unsigned int);
    unsigned char barr[sizeof(unsigned int)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_ulong_write(ch_bin_format_t *bf,unsigned long v){

    size_t sz = sizeof(unsigned long);
    unsigned char barr[sizeof(unsigned long)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_size_write(ch_bin_format_t *bf,size_t v){

    size_t sz = sizeof(size_t);
    unsigned char barr[sizeof(size_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_ssize_write(ch_bin_format_t *bf,ssize_t v){

    size_t sz = sizeof(ssize_t);
    unsigned char barr[sizeof(ssize_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_off_write(ch_bin_format_t *bf,off_t v){

    size_t sz = sizeof(off_t);
    unsigned char barr[sizeof(off_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_int8_write(ch_bin_format_t *bf,int8_t v){

    size_t sz = sizeof(int8_t);
    unsigned char barr[sizeof(int8_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_int16_write(ch_bin_format_t *bf,int16_t v){

    size_t sz = sizeof(int16_t);
    unsigned char barr[sizeof(int16_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_int32_write(ch_bin_format_t *bf,int32_t v){

    size_t sz = sizeof(int32_t);
    unsigned char barr[sizeof(int32_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_int64_write(ch_bin_format_t *bf,int64_t v){

    size_t sz = sizeof(int64_t);
    unsigned char barr[sizeof(int64_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_uint8_write(ch_bin_format_t *bf,uint8_t v){

    size_t sz = sizeof(uint8_t);
    unsigned char barr[sizeof(uint8_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_uint16_write(ch_bin_format_t *bf,uint16_t v){

    size_t sz = sizeof(uint16_t);
    unsigned char barr[sizeof(uint16_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_uint32_write(ch_bin_format_t *bf,uint32_t v){

    size_t sz = sizeof(uint32_t);
    unsigned char barr[sizeof(uint32_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

static inline ssize_t ch_bf_uint64_write(ch_bin_format_t *bf,uint64_t v){

    size_t sz = sizeof(uint64_t);
    unsigned char barr[sizeof(uint64_t)] = {0};
    DO_CHECK(bf,sz);
    DO_VALUE_WRITE(barr,v,sz);
    do_write(bf,barr,sz);
    return (ssize_t)sz;
}

#define F_EXP_BIT_MASK	2139095040
#define F_SIGNIF_BIT_MASK  8388607

static inline int float2_raw_int_bits(float v)
{
	
	int result;
	union{float f;int i;}u;
	u.f=v;
	
	result = u.i;

	if (((result &F_EXP_BIT_MASK) ==F_EXP_BIT_MASK)\
	&&(result & F_SIGNIF_BIT_MASK) != 0)
	     	result = 0x7fc00000;

	return result;

}

#define D_EXP_BIT_MASK	9218868437227405312LL
#define D_SIGNIF_BIT_MASK  4503599627370495LL

static inline int64_t double2_raw_int_bits(double v)
{
	
	int64_t result;
	union{double d;int64_t i;}u;
	u.d=v;
	
	result = u.i;

	if (((result &D_EXP_BIT_MASK) ==D_EXP_BIT_MASK)\
	&&(result & D_SIGNIF_BIT_MASK) != 0LL)
	     	result = 0x7ff8000000000000LL;

	return result;
}

static inline ssize_t ch_bf_float_write(ch_bin_format_t *bf,float v){

    return ch_bf_int_write(bf,float2_raw_int_bits(v));
}

static inline ssize_t ch_bf_double_write(ch_bin_format_t *bf,double v){

    return ch_bf_int64_write(bf,double2_raw_int_bits(v));
}

static inline ssize_t ch_bf_bytes_write(ch_bin_format_t *bf,unsigned char* bytes,size_t len){

    size_t rlen = len+sizeof(size_t);
    DO_CHECK(bf,rlen);
    ch_bf_size_write(bf,len);
    do_write(bf,bytes,len);

    return (ssize_t)rlen;
}

static inline ssize_t ch_bf_string_write(ch_bin_format_t *bf,unsigned char* str){

    size_t len = strlen((const char*)str);
    size_t rlen = len+sizeof(size_t);
    DO_CHECK(bf,rlen);
    ch_bf_size_write(bf,len);
    do_write(bf,str,len);

    return (ssize_t)rlen;
}

/*read basic data type functions*/
#define DO_VALUE_READ(buf, value, byte_num,type)		 	 \
do{	     						 \
	size_t bytes,i;  				 \
    void *buf_p = buf;				 \
	unsigned char b; 					 \
	bytes = (byte_num)*8;			         \
	for (i=1; i<=byte_num; i++)			 \
	{						 \
		b=*((unsigned char*)buf_p);			 \
		buf_p+=1;			         \
		(value) += ((type)(b&0xFF)<<(bytes-i*8));	 \
	}						 \
} while(0)

static inline int  ch_bf_bool_read(ch_bin_format_t *bf){

    int vv = 0;
    if(ch_bf_full(bf,1)){
        return 0;
    }
    DO_VALUE_READ(bf->pos,vv,1,int);
    ch_bf_pos_update(bf,1);
    return vv;
}

static inline short ch_bf_short_read(ch_bin_format_t *bf){

    short vv = 0;
    size_t sz = sizeof(short);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,short);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline int  ch_bf_int_read(ch_bin_format_t *bf){

    int vv = 0;
    size_t sz = sizeof(int);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,int);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline long ch_bf_long_read(ch_bin_format_t *bf){

    long vv = 0;
    size_t sz = sizeof(long);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,long);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline float ch_bf_float_read(ch_bin_format_t *bf){

    union {float f;int i;}u;
    int vv = 0;
    size_t sz = sizeof(int);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,int);
    ch_bf_pos_update(bf,sz);
    u.i = vv;
    return u.f;
}

static inline double ch_bf_double_read(ch_bin_format_t *bf){

    union {double f;int64_t i;}u;
    int64_t vv = 0;
    size_t sz = sizeof(int64_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,int64_t);
    ch_bf_pos_update(bf,sz);
    u.i = vv;
    return u.f;
}

static inline unsigned short ch_bf_ushort_read(ch_bin_format_t *bf){

    unsigned short vv = 0;
    size_t sz = sizeof(unsigned short);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,unsigned short);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline unsigned int ch_bf_uint_read(ch_bin_format_t *bf){

    unsigned int vv = 0;
    size_t sz = sizeof(unsigned int);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,unsigned int);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline unsigned long  ch_bf_ulong_read(ch_bin_format_t *bf){
    
    unsigned long vv = 0;
    size_t sz = sizeof(unsigned long);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,unsigned long);
    ch_bf_pos_update(bf,sz);
    return vv;

}

static inline size_t ch_bf_size_read(ch_bin_format_t *bf){

    size_t vv = 0;
    size_t sz = sizeof(size_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,size_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline ssize_t ch_bf_ssize_read(ch_bin_format_t *bf){

    ssize_t vv = 0;
    size_t sz = sizeof(ssize_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,ssize_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline off_t ch_bf_off_read(ch_bin_format_t *bf){

    off_t vv = 0;
    size_t sz = sizeof(off_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,off_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline int8_t ch_bf_int8_read(ch_bin_format_t *bf){

    int8_t vv = 0;
    size_t sz = sizeof(int8_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,int8_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline int16_t ch_bf_int16_read(ch_bin_format_t *bf){

    int16_t vv = 0;
    size_t sz = sizeof(int16_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,int16_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline int32_t ch_bf_int32_read(ch_bin_format_t *bf){

    int32_t vv = 0;
    size_t sz = sizeof(int32_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,int32_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline int64_t ch_bf_int64_read(ch_bin_format_t *bf){
    int64_t vv = 0;
    size_t sz = sizeof(int64_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,int64_t);
    ch_bf_pos_update(bf,sz);
    return vv;

}

static inline uint8_t ch_bf_uint8_read(ch_bin_format_t *bf){

    uint8_t vv = 0;
    size_t sz = sizeof(uint8_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,uint8_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline uint16_t ch_bf_uint16_read(ch_bin_format_t *bf){

    uint16_t vv = 0;
    size_t sz = sizeof(uint16_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,uint16_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline uint32_t ch_bf_uint32_read(ch_bin_format_t *bf){

    uint32_t vv = 0;
    size_t sz = sizeof(uint32_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,uint32_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline uint64_t ch_bf_uint64_read(ch_bin_format_t *bf){

    uint64_t vv = 0;
    size_t sz = sizeof(uint64_t);

    if(ch_bf_full(bf,sz)){
        return 0;
    }

    DO_VALUE_READ(bf->pos,vv,sz,uint64_t);
    ch_bf_pos_update(bf,sz);
    return vv;
}

static inline void ch_bf_bytes_read(ch_bin_format_t *bf,unsigned char** data,size_t *len){

    *data = NULL;
    *len = 0;

    if(ch_bf_full(bf,sizeof(size_t)))
        return;

    *len = ch_bf_size_read(bf);
    *data = bf->pos;
    ch_bf_pos_update(bf,*len);
}

#endif /*CH_BIN_FORMAT_H*/
