/*************************************************************************
 > File Name: ch_file_store.h
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年7月15日 星期五 11时17分29秒
 ************************************************************************/

#ifndef CH_FILE_STORE_H
#define CH_FILE_STORE_H

typedef struct ch_file_store_t ch_file_store_t;

#include "ch_fpath.h"

struct ch_file_store_t {

	ch_fpath_t fpath;

	struct {
		void *start;
		void *pos;
		void *end;
	}buf;
};

#define FILE_STORE_EMPTY(fstore) ((fstore)->buf.start == (fstore)->buf.pos)
#define FILE_STORE_FULL(fstore,sz) ((fstore)->buf.pos+(sz)>(fstore)->buf.end) 
#define FILE_STORE_LARGE_DATA(fstore,sz) ((fstore)->buf.start+(sz)>(fstore)->buf.end)
#define FILE_STORE_UPDATE(fstore,sz) ((fstore)->buf.pos = (fstore)->buf.pos+(sz))
#define FILE_STORE_RESET(fstore) ((fstore)->buf.pos = (fstore)->buf.start)
#define FILE_STORE_CONTENT(fstore) ((fstore)->buf.start)
#define FILE_STORE_CONTENT_SIZE(fstore) (size_t)((fstore)->buf.pos-(fstore)->buf.start)

#define FILE_STORE_WRITE(fstore,data,dlen) do { \
    memcpy(fstore->buf.pos,data,dlen);			\
    FILE_STORE_UPDATE(fstore,dlen);				\
}while(0)

extern ch_file_store_t * ch_file_store_create(ch_pool_t *mp,const char *root_dir,size_t bsize,int create_dir_type, int log_id);

extern int  ch_file_store_write(ch_file_store_t *fstore,void *data,size_t dlen);

extern void ch_file_store_flush(ch_file_store_t *fstore);

extern void ch_file_store_close(ch_file_store_t *fstore);

#endif /*CH_FILE_STORE_H*/
