/*
 *
 *      Filename: ch_pp_data_input.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 19:35:11
 * Last Modified: 2018-05-14 19:35:11
 */

#ifndef CH_PP_DATA_INPUT_H
#define CH_PP_DATA_INPUT_H

typedef struct ch_pp_data_input_t ch_pp_data_input_t;
typedef struct ch_pp_data_line_t ch_pp_data_line_t;

#include "ch_buffer.h"
#include "ch_mpool.h"

struct ch_pp_data_input_t {

	ch_buffer_t *g_buffer;
	
	ch_pool_t *mp;

	unsigned char *base;
	unsigned char *pos;
	unsigned char *end;

	struct {
	
		unsigned char *data;
		size_t dlen;

		unsigned char *addr;
		unsigned char *addr_end;

	} remain_data;

};

struct ch_pp_data_line_t {

	unsigned char *line;
	size_t len;
	size_t rawlen;
};

#define ch_pp_din_pos_update(din,sz) ((din)->pos = (din)->pos+(sz))
#define ch_pp_din_has_data(din,sz) ((din)->pos+(sz)<(din)->end)
#define ch_pp_din_rdlen(din) (((din)->pos<(din)->end)?((din)->end-(din)->pos):0)
#define ch_pp_din_base(din) ((din)->base)
#define ch_pp_din_pos(din) ((din)->pos)
#define ch_pp_din_pos_offset(din) ((din)->pos-(din)->base)
#define ch_pp_din_reset(din) ((din)->pos = (din)->base)
#define ch_pp_din_skip_to_end(din) ((din)->pos = (din)->end)

#define ch_pp_din_has_remain_data(din) ((din)->remain_data.data == NULL || (din)->remain_data.dlen == 0)
#define ch_pp_din_need_alloc_new_store(din,sz) ((din)->remain_data.addr == NULL ||(din)->remain_data.addr+(sz)>(din)->remain_data.addr_end)

#define ch_pp_din_init(din,b,mp)do {		 \
	(din)->g_buffer = b;						\
	(din)->base = NULL;						 \
	(din)->pos = NULL;						 \
	(din)->end = NULL;						 \
	(din)->mp = mp;							 \
	(din)->remain_data.data = NULL;			 \
	(din)->remain_data.dlen = 0;				 \
	(din)->remain_data.addr = NULL;			\
	(din)->remain_data.addr_end = NULL;		\
}while(0)

/*read basic data type functions*/
#define DIN_VALUE_READ(buf, value, byte_num,type)		 \
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

#define SAFE_SIZE sizeof(unsigned char*)

static inline uint32_t ch_pp_din_uint32_read(ch_pp_data_input_t *din){

    uint32_t vv = 0;
    size_t sz = sizeof(uint32_t);

    DIN_VALUE_READ(din->pos,vv,sz,uint32_t);

    ch_pp_din_pos_update(din,sz);

    return vv;
}

static inline uint8_t ch_pp_din_uint8_read(ch_pp_data_input_t *din){

    uint8_t vv = (uint8_t)(*din->pos);

    ch_pp_din_pos_update(din,1);

    return vv;
}

static inline int ch_pp_din_read_prefare(ch_pp_data_input_t *din,void *data,size_t dlen){

	size_t ttsize = 0;
	size_t rdlen = din->remain_data.dlen;
	unsigned char *rdata = din->remain_data.data;
	unsigned char *p;

	if(rdlen == 0||rdata == NULL){
	
		/*no remain data*/
		din->base = (unsigned char*)data;
		din->pos = din->base;
		din->end = din->base+dlen; 

		return 0;
	}

	/*has some remain data*/
	ttsize = rdlen+dlen;
	if(ch_buffer_need_update(din->g_buffer,ttsize)){
	
		if(ch_buffer_update(din->g_buffer,ttsize))
			return -1;
	}

	p = (unsigned char*)din->g_buffer->bdata;
	memcpy(p,rdata,rdlen);
	p += rdlen;
	memcpy(p,data,dlen);

	din->base = (unsigned char*)din->g_buffer->bdata;
	din->pos = din->base;
	din->end = din->base+ttsize;

	/*reset remain data*/
	din->remain_data.data = NULL;
	din->remain_data.dlen = 0;

	return 0;
}


static inline int ch_pp_din_read_end(ch_pp_data_input_t *din){

	size_t rdlen = ch_pp_din_rdlen(din);
	if(rdlen>0){
		/*have some remain data need to be store*/
		if(ch_pp_din_need_alloc_new_store(din,rdlen)){
		
			din->remain_data.addr = ch_pcalloc(din->mp,rdlen+SAFE_SIZE);
			if(din->remain_data.addr == NULL){
			
				return -1;
			}
			din->remain_data.addr_end = din->remain_data.addr+rdlen;
		}

		/*store remain data*/
		memcpy(din->remain_data.addr,ch_pp_din_pos(din),rdlen);
		din->remain_data.data = din->remain_data.addr;
		din->remain_data.dlen = rdlen;
	}else{
	
		din->remain_data.data = NULL;
		din->remain_data.dlen = 0;
	}

	ch_pp_din_reset(din);

	return 0;
}

static inline int ch_pp_data_input_line_read(ch_pp_data_input_t *din,ch_pp_data_line_t *dline){


	unsigned char *pos = din->pos;
	unsigned char *end = din->end;

	size_t lineLen =0;
	size_t rawlineLen = 0;
	unsigned char *p,*l_end = NULL;	
	dline->line = NULL;
	dline->len = 0;
	dline->rawlen = 0;

 // attempt to find end of line (LF)

	for ( p = pos; p < end; p++) {
		if (*p == LF) {
			 l_end = p;
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

	ch_pp_din_pos_update(din,rawlineLen);

	dline->line = pos;
	dline->len = lineLen;
	dline->rawlen = rawlineLen;

	return 0;
}

#endif /*CH_PP_DATA_INPUT_H*/
