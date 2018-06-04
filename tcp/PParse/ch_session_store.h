/*
 *
 *      Filename: ch_session_store.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-17 10:52:52
 * Last Modified: 2018-05-17 10:52:52
 */

#ifndef CH_SESSION_STORE_H
#define CH_SESSION_STORE_H

typedef struct ch_session_store_t ch_session_store_t;

#include <msgpack.h>
#include "ch_file_store.h"
#include "ch_session_entry.h"
#include "ch_shm_format.h"

struct ch_session_store_t {

	ch_file_store_t *fstore;

	ch_shm_format_t *shm_fmt;

	ch_pp_context_t *pcontext;

	msgpack_packer pk;
    msgpack_sbuffer pk_buf;

};

extern ch_session_store_t * ch_session_store_create(ch_pool_t *mp,ch_pp_context_t *pcontext);

extern int ch_session_store_put(ch_session_entry_t *sentry,void *session);

extern void ch_session_store_destroy(ch_session_store_t *sstore);



static inline void ch_msgpack_write_str(msgpack_packer *pk,const char *str){

    size_t len = strlen(str);
    msgpack_pack_str(pk,len);
    msgpack_pack_str_body(pk,str,len);
}

static inline void ch_msgpack_write_kv(msgpack_packer *pk,const char *k,const char *v){
    
    ch_msgpack_write_str(pk,k);
    ch_msgpack_write_str(pk,v==NULL?"":v);
}

static inline void ch_msgpack_write_number_str(msgpack_packer *pk,const char *k,unsigned long number){                                               
                                                                                                                                       
    char b[128]={0};                                                                                                                   
    snprintf(b,127,"%lu",number);                                                                                                      
    ch_msgpack_write_kv(pk,k,(const char *)b);                                                                                            
}                                                                                                                                      

static inline int _str_empty(const char *v){

    return v == NULL || strlen(v) == 0;
}

static inline void ch_msgpack_map_start(msgpack_packer *pk,const char *k,size_t n){

    if(_str_empty(k)==0){
        ch_msgpack_write_str(pk,k); 
    }
    msgpack_pack_map(pk,n);
}

static inline void ch_msgpack_write_uint(msgpack_packer *pk,const char *k,unsigned int v){
    
	if(_str_empty(k)==0){
		ch_msgpack_write_str(pk,k);
	}
	
	msgpack_pack_unsigned_int(pk,v);
}

static inline void ch_msgpack_write_uint8(msgpack_packer *pk,const char *k,uint8_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_write_str(pk,k);
	}
	
	msgpack_pack_uint8(pk,v);
}

static inline void ch_msgpack_write_uint16(msgpack_packer *pk,const char *k,uint16_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_write_str(pk,k);
	}
	
	msgpack_pack_uint16(pk,v);
}

static inline void ch_msgpack_write_uint32(msgpack_packer *pk,const char *k,uint32_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_write_str(pk,k);
	}
	
	msgpack_pack_uint32(pk,v);
}

static inline void ch_msgpack_write_uint64(msgpack_packer *pk,const char *k,uint64_t v){
    
	if(_str_empty(k)==0){
		ch_msgpack_write_str(pk,k);
	}
	
	msgpack_pack_uint64(pk,v);
}

static inline void ch_msgpack_array_start(msgpack_packer *pk,const char *k,size_t n){

    if(_str_empty(k)==0){
        ch_msgpack_write_str(pk,k); 
    }
    msgpack_pack_array(pk,n);
}


#endif /*CH_SESSION_STORE_H*/
