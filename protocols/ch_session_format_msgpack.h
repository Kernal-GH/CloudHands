/*
 * =====================================================================================
 *
 *       Filename:  ch_session_format_msgpack.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月18日 18时39分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SESSION_FORMAT_MSGPACK_H
#define CH_SESSION_FORMAT_MSGPACK_H

typedef struct ch_session_format_msgpack_t ch_session_format_msgpack_t;

#include <msgpack.h>
#include "ch_session_format.h"

struct ch_session_format_msgpack_t {

    ch_session_format_t fmt;
    msgpack_packer pk;
    msgpack_sbuffer pk_buf;

};

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

extern ch_session_format_t * ch_session_format_msgpack_create(apr_pool_t *mp,
	    ch_file_store_t *fstore);

extern void ch_session_format_msgpack_destroy(ch_session_format_t *fmt);

#endif /*CH_SESSION_FORMAT_MSGPACK_H*/
