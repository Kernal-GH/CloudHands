/*
 *
 *      Filename: ch_data_input.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-24 12:14:16
 * Last Modified: 2018-04-24 12:14:16
 */

#ifndef CH_DATA_INPUT_H
#define CH_DATA_INPUT_H

typedef struct ch_data_input_t ch_data_input_t;
typedef struct ch_data_buffer_t ch_data_buffer_t;
typedef struct ch_data_line_t ch_data_line_t;

#include "ch_constants.h"

struct ch_data_line_t {

	char *line;
	size_t len;
};

struct ch_data_buffer_t {

	void *start;
	void *pos;
	void *last;
	void *end;
};


struct ch_data_input_t {

	ch_data_buffer_t dbuf;
	
	struct {
	
		void *data;
		size_t dlen;

	}remain_data;

	int is_alloc;
};


#define ch_data_input_pos_update(din,sz) (din->dbuf.pos=din->dbuf.pos+sz)
#define ch_data_input_read_over(din,sz) (din->dbuf.pos+sz>din->dbuf.last)
#define ch_data_input_pos(din) (din->dbuf.pos)
#define ch_data_input_base(din) (din->dbuf.start)
#define ch_data_input_last(din) (din->dbuf.last)
#define ch_data_input_dlen(din) (din->dbuf.last-din->dbuf.start) 
#define ch_data_input_rdlen(din) (din->dbuf.last-din->dbuf.pos)
#define ch_data_input_offset_pos(din) (din->dbuf.pos-din->dbuf.start)
#define ch_data_input_offset_last(din) (din->dbuf.last-din->dbuf.start)

#define ch_data_input_remain_data_free(din) do{ \
	free(din->remain_data.data);				\
	din->remain_data.data = NULL;				\
	din->remain_data.dlen = 0;					\
}while(0)


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

static inline void ch_data_input_init(ch_data_input_t *din){

	ch_data_buffer_t *dbuf = &din->dbuf;
	dbuf->start = NULL;
	dbuf->pos = NULL;
	dbuf->last = NULL;
	dbuf->end = NULL;

	din->is_alloc = 0;
	din->remain_data.data = NULL;
	din->remain_data.dlen = 0;
}

static inline int ch_data_input_read_start(ch_data_input_t *din,void *start,size_t bsize,size_t psize){

	ch_data_buffer_t *dbuf = &din->dbuf;

	size_t remain_size = din->remain_data.dlen;

	if(remain_size){
		
		if(start+psize+remain_size<=start+bsize){
			
			memcpy(start+remain_size,start,psize);
			memcpy(start,din->remain_data.data,remain_size);
			dbuf->start = start;
			dbuf->pos = dbuf->start;
			dbuf->last = dbuf->start+psize+remain_size;
			dbuf->end = dbuf->start+bsize;

		}else{
		
			void *p = malloc(psize+remain_size+sizeof(void*));
			if(p == NULL)
				return -1;

			din->is_alloc = 1;

			memcpy(p,din->remain_data.data,remain_size);
			memcpy(p+remain_size,start,psize);
			dbuf->start = p;
			dbuf->pos = dbuf->start;
			dbuf->last = dbuf->start+psize+remain_size;
			dbuf->end = dbuf->last;
		}
		/*free remain data*/
		ch_data_input_remain_data_free(din);

	}else{
	
		dbuf->start = start;
		dbuf->pos = start;
		dbuf->last = start+psize;
		dbuf->end = start+bsize;
	}

	return 0;
} 

static inline int ch_data_input_read_stop(ch_data_input_t *din){

	ch_data_buffer_t *dbuf = &din->dbuf;
	size_t remain_size = dbuf->last-dbuf->pos;

	if(remain_size){
	
		if(din->is_alloc){
		
			if(dbuf->pos!=dbuf->start)
				memcpy(dbuf->start,dbuf->pos,remain_size);

			din->remain_data.data = dbuf->start;
			din->remain_data.dlen = remain_size;
			din->is_alloc = 0;
			return 0;
		}

		void *p = malloc(remain_size+sizeof(void*));
		if(p == NULL)
			return -1;

		memcpy(p,dbuf->pos,remain_size);
		din->remain_data.data = p;
		din->remain_data.dlen = remain_size;
	}else{
	
		din->remain_data.data = NULL;
		din->remain_data.dlen = 0;
	}
	
	if(din->is_alloc)
		free(dbuf->start);

	dbuf->start = NULL;
	dbuf->pos = NULL;
	dbuf->end = NULL;
	dbuf->last = NULL;

	return 0;
}

static inline int ch_data_input_bool_read(ch_data_input_t *din) { 
    int vv = 0;
    
	if(ch_data_input_read_over(din,1)){
        return 0;
    }
    
	VALUE_READ(din->dbuf.pos,vv,1,int);
    ch_data_input_pos_update(din,1);
    return vv;
}

static inline short ch_data_input_short_read(ch_data_input_t *din){

    short vv = 0;
    size_t sz = sizeof(short);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,short);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline int  ch_data_input_int_read(ch_data_input_t *din){

    int vv = 0;
    size_t sz = sizeof(int);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,int);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline long ch_data_input_long_read(ch_data_input_t *din){

    long vv = 0;
    size_t sz = sizeof(long);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,long);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline float ch_data_input_float_read(ch_data_input_t *din){

    union {float f;int i;}u;
    int vv = 0;
    size_t sz = sizeof(int);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,int);
    ch_data_input_pos_update(din,sz);
    u.i = vv;
    return u.f;
}

static inline double ch_data_input_double_read(ch_data_input_t *din){

    union {double f;int64_t i;}u;
    int64_t vv = 0;
    size_t sz = sizeof(int64_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,int64_t);
    ch_data_input_pos_update(din,sz);
    u.i = vv;
    return u.f;
}

static inline unsigned short ch_data_input_ushort_read(ch_data_input_t *din){

    unsigned short vv = 0;
    size_t sz = sizeof(unsigned short);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,unsigned short);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline unsigned int ch_data_input_uint_read(ch_data_input_t *din){

    unsigned int vv = 0;
    size_t sz = sizeof(unsigned int);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,unsigned int);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline unsigned long  ch_data_input_ulong_read(ch_data_input_t *din){
    
    unsigned long vv = 0;
    size_t sz = sizeof(unsigned long);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,unsigned long);
    ch_data_input_pos_update(din,sz);
    return vv;

}

static inline size_t ch_data_input_size_read(ch_data_input_t *din){

    size_t vv = 0;
    size_t sz = sizeof(size_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,size_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline ssize_t ch_data_input_ssize_read(ch_data_input_t *din){

    ssize_t vv = 0;
    size_t sz = sizeof(ssize_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,ssize_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline off_t ch_data_input_off_read(ch_data_input_t *din){

    off_t vv = 0;
    size_t sz = sizeof(off_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,off_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline int8_t ch_data_input_int8_read(ch_data_input_t *din){

    int8_t vv = 0;
    size_t sz = sizeof(int8_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,int8_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline int16_t ch_data_input_int16_read(ch_data_input_t *din){

    int16_t vv = 0;
    size_t sz = sizeof(int16_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,int16_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline int32_t ch_data_input_int32_read(ch_data_input_t *din){

    int32_t vv = 0;
    size_t sz = sizeof(int32_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,int32_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline int64_t ch_data_input_int64_read(ch_data_input_t *din){
    int64_t vv = 0;
    size_t sz = sizeof(int64_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,int64_t);
    ch_data_input_pos_update(din,sz);
    return vv;

}

static inline uint8_t ch_data_input_uint8_read(ch_data_input_t *din){

    uint8_t vv = 0;
    size_t sz = sizeof(uint8_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,uint8_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline uint16_t ch_data_input_uint16_read(ch_data_input_t *din){

    uint16_t vv = 0;
    size_t sz = sizeof(uint16_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,uint16_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline uint32_t ch_data_input_uint32_read(ch_data_input_t *din){

    uint32_t vv = 0;
    size_t sz = sizeof(uint32_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,uint32_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline uint64_t ch_data_input_uint64_read(ch_data_input_t *din){

    uint64_t vv = 0;
    size_t sz = sizeof(uint64_t);

    if(ch_data_input_read_over(din,sz)){
        return 0;
    }

    VALUE_READ(din->dbuf.pos,vv,sz,uint64_t);
    ch_data_input_pos_update(din,sz);
    return vv;
}

static inline void ch_data_input_bytes_read(ch_data_input_t *din,unsigned char** data,size_t *len){

    *data = NULL;
    *len = 0;

    if(ch_data_input_read_over(din,sizeof(size_t)))
        return;

    *len = ch_data_input_size_read(din);
    *data = din->dbuf.pos;
    ch_data_input_pos_update(din,*len);
}

static inline void ch_data_input_bstring_read(ch_data_input_t *din,unsigned char** data,uint8_t *len){

    *data = NULL;
    *len = 0;

    if(ch_data_input_read_over(din,sizeof(uint8_t)))
        return;

    *len = ch_data_input_uint8_read(din);

    *data = din->dbuf.pos;

    ch_data_input_pos_update(din,*len);
}

static inline int ch_data_input_line_read(ch_data_input_t *din,ch_data_line_t *dline){


	char *pos = (char*)din->dbuf.pos;
	char *end = (char*)din->dbuf.last;
	size_t lineLen =0;
	size_t rawlineLen = 0;
	char *p,*l_end = NULL;	
	dline->line = NULL;
	dline->len = 0;
	
 // attempt to find end of line (LF)

	for ( p = pos; p < end; p++) {
		if (*p == LF) {
			 l_end = pos;
			 break;
		}
	}
	
	if (l_end == NULL) {
		/*no find a complete line*/
		return -1;
	}
	
	lineLen = l_end - pos + 1;
	rawlineLen = lineLen;

	// discard LF if found
	lineLen--;
	// discard CR if found
	if (lineLen > 0 && *(l_end-1) == CR)
		lineLen--;
	
	ch_data_input_pos_update(din,rawlineLen);

	dline->line = pos;
	dline->len = lineLen;
	
	return 0;
}

#endif /*CH_DATA_INPUT_H*/
