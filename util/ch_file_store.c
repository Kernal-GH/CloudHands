/*************************************************************************
 > File Name: ch_file_store.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年7月15日 星期五 15时51分29秒
 ************************************************************************/

#include "ch_constants.h"
#include "ch_log.h"
#include "ch_file_store.h"
#include "ch_util.h"

static inline int _fstore_buf_empty(ch_file_store_t *fstore){

    return fstore->buf.start == fstore->buf.pos;
}

static inline int _fstore_buf_full(ch_file_store_t *fstore,size_t sz){

    return fstore->buf.pos+sz>fstore->buf.end;
}

static inline int _fstore_large_data(ch_file_store_t *fstore,size_t sz){

    return sz>=(size_t)(fstore->buf.end-fstore->buf.start);
}

static inline void _fstore_buf_update(ch_file_store_t *fstore,size_t sz){

    fstore->buf.pos = fstore->buf.pos+sz;
}

static inline void _fstore_buf_reset(ch_file_store_t *fstore){

    fstore->buf.pos = fstore->buf.start;
    fstore->buf.last = fstore->buf.start;
}

static int _fstore_file_write(ch_file_store_t *fstore,void *data,size_t dlen){

	char *fname,*fname_tmp;

	ch_fpath_name_create(&fstore->fpath,&fname,&fname_tmp);

    FILE *fp = fopen(fname_tmp,"w+");
    if(fp == NULL){
    
        ch_log(CH_LOG_ERR,"can not open the file:%s for writting!",fname_tmp);
        return -1;
    }
    
    fwrite(data,dlen,1,fp);
    fclose(fp);
    
    rename(fname_tmp,fname);

    return 0;
}

static inline void _fstore_buf_write(ch_file_store_t *fstore,void *data,size_t dlen){

    memcpy(fstore->buf.pos,data,dlen);
    _fstore_buf_update(fstore,dlen);
}

ch_file_store_t * 
ch_file_store_create(apr_pool_t *mp,const char *root_dir,size_t bsize,int create_dir_type, int log_id){

    ch_file_store_t *fstore = NULL;

    if(root_dir == NULL || bsize == 0){
    
        ch_log(CH_LOG_ERR,"Invalid args for create file store!");
        return NULL;
    }

    fstore = (ch_file_store_t*)apr_palloc(mp,sizeof(*fstore));

    /*create fname buf*/

    if(ch_fpath_init(&fstore->fpath,root_dir,create_dir_type,1, log_id)){
    
        ch_log(CH_LOG_ERR,"Init file path failed!");
        return NULL;
    }
    
    /*create a buf*/
    fstore->buf.start = malloc(bsize);
    if(fstore->buf.start == NULL){
    
        ch_log(CH_LOG_ERR,"no free memory used to create buf for file store!");
        return NULL;
    }

    fstore->buf.pos = fstore->buf.start;
    fstore->buf.last = fstore->buf.start;
    fstore->buf.end = fstore->buf.start+bsize;

    return fstore;
}

int  ch_file_store_write(ch_file_store_t *fstore,void *data,size_t dlen){

    if(_fstore_large_data(fstore,dlen)){
    
        _fstore_file_write(fstore,data,dlen);
        return 0;
    }

    if(_fstore_buf_full(fstore,dlen)){
    
        ch_file_store_flush(fstore);
    }
    
    _fstore_buf_write(fstore,data,dlen);

    return 0;
}

void ch_file_store_flush(ch_file_store_t *fstore){

    if(_fstore_buf_empty(fstore) == 0){
   

        _fstore_file_write(fstore,fstore->buf.start,(size_t)(fstore->buf.pos-fstore->buf.start));
        _fstore_buf_reset(fstore);
    }

}

void ch_file_store_close(ch_file_store_t *fstore){

    ch_file_store_flush(fstore);
    free(fstore->buf.start);

	ch_fpath_fin(&fstore->fpath);
}
