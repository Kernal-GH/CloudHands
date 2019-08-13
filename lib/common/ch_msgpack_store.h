/*
 * =====================================================================================
 *
 *       Filename:  ch_msgpack_store.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/12/2019 09:45:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_MSGPACK_STORE_H
#define CH_MSGPACK_STORE_H

typedef struct ch_msgpack_store_t ch_msgpack_store_t;

#include "ch_mpool.h"
#include <msgpack.h>

struct ch_msgpack_store_t {

    ch_pool_t *mp;

    msgpack_packer pk;
    msgpack_sbuffer pk_buf;

};

extern ch_msgpack_store_t* ch_msgpack_store_create(ch_pool_t *mp);


extern void ch_msgpack_store_destroy(ch_msgpack_store_t *dstore);

static inline void ch_msgpack_store_reset(ch_msgpack_store_t *dstore) {

    msgpack_sbuffer_clear(&dstore->pk_buf);
}

static inline int _str_empty(const char *v){

    return v == NULL || strlen(v) == 0;
}

static inline void ch_msgpack_store_write_str(ch_msgpack_store_t *dstore,const char *str){

    size_t len = strlen(str);
    msgpack_pack_str(&dstore->pk,len);
    msgpack_pack_str_body(&dstore->pk,str,len);
}

static inline void ch_msgpack_store_write_str_wlen(ch_msgpack_store_t *dstore,const char *k,const char *str,size_t len){

	ch_msgpack_store_write_str(dstore,k);
    msgpack_pack_str(&dstore->pk,len);
    msgpack_pack_str_body(&dstore->pk,str==NULL||len==0?"":str,len);
}

static inline void ch_msgpack_store_write_bin(ch_msgpack_store_t *dstore,void *data,size_t dlen){

    msgpack_pack_bin(&dstore->pk,dlen);
    msgpack_pack_bin_body(&dstore->pk,data,dlen);
}

static inline void ch_msgpack_store_write_bin_kv(ch_msgpack_store_t *dstore,const char *k,void *data,size_t dlen){
    
	if(_str_empty(k)==0){
		ch_msgpack_store_write_str(dstore,k);
	}
	
	ch_msgpack_store_write_bin(dstore,data,dlen);

}

static inline void ch_msgpack_store_write_kv(ch_msgpack_store_t *dstore,const char *k,const char *v){
    
    ch_msgpack_store_write_str(dstore,k);
    ch_msgpack_store_write_str(dstore,v==NULL?"":v);
}

static inline void ch_msgpack_store_write_number_str(ch_msgpack_store_t *dstore,const char *k,unsigned long number){                                               
                                                                                                                                       
    char b[128]={0};                                                                                                                   
    snprintf(b,127,"%lu",number);                                                                                                      
    ch_msgpack_store_write_kv(dstore,k,(const char *)b);                                                                                            
}                                                                                                                                      



static inline void ch_msgpack_store_map_start(ch_msgpack_store_t *dstore,const char *k,size_t n){

    if(_str_empty(k)==0){
        ch_msgpack_store_write_str(dstore,k); 
    }
    msgpack_pack_map(&dstore->pk,n);
}

static inline void ch_msgpack_store_write_uint(ch_msgpack_store_t *dstore,const char *k,unsigned int v){
    
	if(_str_empty(k)==0){
		ch_msgpack_store_write_str(dstore,k);
	}
	
	msgpack_pack_unsigned_int(&dstore->pk,v);
}

static inline void ch_msgpack_store_write_uint8(ch_msgpack_store_t *dstore,const char *k,uint8_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_store_write_str(dstore,k);
	}
	
	msgpack_pack_uint8(&dstore->pk,v);
}

static inline void ch_msgpack_store_write_uint16(ch_msgpack_store_t *dstore,const char *k,uint16_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_store_write_str(dstore,k);
	}
	
	msgpack_pack_uint16(&dstore->pk,v);
}

static inline void ch_msgpack_store_write_uint32(ch_msgpack_store_t *dstore,const char *k,uint32_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_store_write_str(dstore,k);
	}
	
	msgpack_pack_uint32(&dstore->pk,v);
}

static inline void ch_msgpack_store_write_uint64(ch_msgpack_store_t *dstore,const char *k,uint64_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_store_write_str(dstore,k);
	}
	
	msgpack_pack_uint64(&dstore->pk,v);
}

static inline void ch_msgpack_store_array_start(ch_msgpack_store_t *dstore,const char *k,size_t n){

    if(_str_empty(k)==0){
        ch_msgpack_store_write_str(dstore,k); 
    }
    msgpack_pack_array(&dstore->pk,n);
}

#endif /*CH_MSGPACK_STORE_H */
