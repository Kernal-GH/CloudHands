/*************************************************************************
 > File Name: ch_file_store.h
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年7月15日 星期五 11时17分29秒
 ************************************************************************/

#ifndef CH_FILE_STORE_H
#define CH_FILE_STORE_H

typedef struct ch_file_store_t ch_file_store_t;

#include <apr_pools.h>
#include "ch_buf.h"
#include "ch_fpath.h"

struct ch_file_store_t {

    ch_buf_t buf;

	ch_fpath_t fpath;
};
 
extern ch_file_store_t * ch_file_store_create(apr_pool_t *mp,const char *root_dir,size_t bsize,int create_dir_type, int log_id);

extern int  ch_file_store_write(ch_file_store_t *fstore,void *data,size_t dlen);

extern void ch_file_store_flush(ch_file_store_t *fstore);

extern void ch_file_store_close(ch_file_store_t *fstore);

#endif /*CH_FILE_STORE_H*/
