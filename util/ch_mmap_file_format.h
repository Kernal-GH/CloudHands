/*************************************************************************
 > File Name: ch_mmap_file_format.h
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月16日 星期四 10时25分29秒
 ************************************************************************/

#ifndef CH_MMAP_FILE_FORMAT_H
#define CH_MMAP_FILE_FORMAT_H

typedef struct ch_mmap_file_format_t ch_mmap_file_format_t;
typedef struct ch_mmap_file_header_t ch_mmap_file_header_t;
typedef struct ch_mmap_file_entry_t  ch_mmap_file_entry_t;
typedef struct ch_mmap_file_iterator_t ch_mmap_file_iterator_t;

#include <apr_pools.h>
#include "ch_mmap.h"
#include "ch_bin_format.h"

#define FIMAGIC 0XFFFA
#define EIMAGIC 0XFFFB
#define MAJOR_VERSION  1
#define MINJOR_VERSION 0

struct ch_mmap_file_header_t {
    ch_mmap_buf_t mmb;
    ch_bin_format_t bfmt;
    uint32_t magic;
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t entries_number;
    uint32_t entries_size;
};
#define CH_MMAP_FILE_HEADER_LEN (sizeof(uint32_t)*5)

struct ch_mmap_file_format_t {
    ch_mmap_file_header_t cur_fheader;
    apr_pool_t *mp;
    ch_mmap_t *mm;
    int has_fheader;
    uint64_t entry_size;
};



struct ch_mmap_file_entry_t {
    uint32_t magic;
    uint32_t entry_size;

    
    uint8_t  packet_type;
    uint8_t  session_direct;
    uint32_t protocol_id;
    uint64_t session_id;

    uint64_t time;
    
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;

    uint32_t data_offset;
    size_t data_len;
    void *data;
};
#define CH_MMAP_FILE_ENTRY_HEADER_LEN (sizeof(uint8_t)*2+sizeof(uint16_t)*2+sizeof(uint32_t)*6+sizeof(uint64_t)*2+sizeof(size_t))

static inline uint32_t ch_mmap_file_entry_len(ch_mmap_file_entry_t *fentry){

    return fentry->data_len+ CH_MMAP_FILE_ENTRY_HEADER_LEN;
}

struct ch_mmap_file_iterator_t {

    ch_mmap_file_header_t *fheader;
    ch_mmap_file_entry_t * (*next)(ch_mmap_file_iterator_t *iter);
    uint32_t entries_count;
};

extern ch_mmap_file_format_t * 
ch_mmap_file_format_create(apr_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,int is_write);

extern void ch_mmap_file_format_destroy(ch_mmap_file_format_t *mmfmt);

extern int ch_mmap_file_format_put(ch_mmap_file_format_t *mmfmt,ch_mmap_file_entry_t *fentry);

extern ch_mmap_file_iterator_t *ch_mmap_file_format_iterator_prefare(ch_mmap_file_format_t *mmfmt);

extern void ch_mmap_file_format_iterator_commit(ch_mmap_file_format_t *mmfmt,ch_mmap_file_iterator_t *iter);

#endif /*CH_MMAP_FILE_FORMAT_H*/
