/*
 *
 *      Filename: ch_proto_session_store.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-28 13:26:51
 * Last Modified: 2018-07-28 13:26:51
 */

#ifndef CH_PROTO_SESSION_STORE_H
#define CH_PROTO_SESSION_STORE_H

typedef struct ch_proto_session_store_t ch_proto_session_store_t;
typedef struct ch_proto_body_store_t ch_proto_body_store_t;

#include <msgpack.h>
#include "ch_shm_format.h"
#include "ch_fpath.h"
#include "ch_mpool.h"
#include "ch_string.h"
#include "ch_tcp_session.h"
#include "ch_buffer.h"
#include "ch_proto_session_entry.h"


struct ch_proto_body_store_t {

	ch_fpath_t *req_fpath;
	ch_fpath_t *res_fpath;
};

struct ch_proto_session_store_t {

	uint32_t task_id;

	ch_pool_t *mp;

	ch_shm_format_t *other_shm_fmt;

	ch_shm_format_t *http_shm_fmt;

	uint64_t shm_last_time;
	uint64_t shm_flush_timeout;

    ch_buffer_t g_buffer;

	msgpack_packer pk;
    
	msgpack_sbuffer pk_buf;

	ch_proto_body_store_t body_stores[PROTOCOL_MAX];

};

extern ch_proto_session_store_t *ch_proto_session_store_create(ch_pool_t *mp,uint32_t task_id,const char *shm_fname,
	uint64_t shm_fsize,uint64_t fentry_size,uint64_t shm_flush_timeout);


extern void ch_proto_session_store_destroy(ch_proto_session_store_t *pstore);

extern int ch_proto_session_store_write(ch_proto_session_store_t *pstore,ch_tcp_session_t *tsession,ch_proto_session_entry_t *s);

static inline ch_proto_body_store_t * ch_proto_store_body_find(ch_proto_session_store_t *pstore,uint32_t proto_id){

	ch_proto_body_store_t *bstore;

	if(proto_id >= PROTOCOL_MAX)
		return NULL;

	bstore = &pstore->body_stores[proto_id];

	if(bstore->req_fpath == NULL && bstore->res_fpath == NULL)
		return NULL;

	return bstore;
}

static inline ch_proto_body_store_t * ch_proto_store_body_find_create(ch_proto_session_store_t *pstore,const char *req_store_dir,
	const char *res_store_dir,int create_dir_type,uint32_t proto_id){

	ch_proto_body_store_t *bstore;

	if(proto_id >= PROTOCOL_MAX)
		return NULL;

	bstore = &pstore->body_stores[proto_id];

	if(bstore->req_fpath == NULL && bstore->res_fpath == NULL){

        if(req_store_dir){
            bstore->req_fpath = ch_fpath_create(pstore->mp,
                    (const char*)ch_psprintf(pstore->mp,"%s/%lu/",req_store_dir,pstore->task_id),
			    create_dir_type,0,0);
            if(bstore->req_fpath == NULL)
                return NULL;
        }

        if(res_store_dir){
            bstore->res_fpath = ch_fpath_create(pstore->mp,
			    (const char*)ch_psprintf(pstore->mp,"%s/%lu/",res_store_dir,pstore->task_id),
			    create_dir_type,0,0);
            
            if(bstore->res_fpath == NULL)
                return NULL;
        }

    }

	return bstore;

}
static inline int _str_empty(const char *v){

    return v == NULL || strlen(v) == 0;
}

static inline void ch_msgpack_write_str(msgpack_packer *pk,const char *str){

    size_t len = strlen(str);
    msgpack_pack_str(pk,len);
    msgpack_pack_str_body(pk,str,len);
}

static inline void ch_msgpack_write_bin(msgpack_packer *pk,void *data,size_t dlen){

    msgpack_pack_bin(pk,dlen);
    msgpack_pack_bin_body(pk,data,dlen);
}

static inline void ch_msgpack_write_bin_kv(msgpack_packer *pk,const char *k,void *data,size_t dlen){
    
	if(_str_empty(k)==0){
		ch_msgpack_write_str(pk,k);
	}
	
	ch_msgpack_write_bin(pk,data,dlen);

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

#endif /*CH_PROTO_SESSION_STORE_H*/
