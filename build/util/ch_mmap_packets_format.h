/*
 *
 *      Filename: ch_mmap_packets_format.h
 *
 *        Author: jacks001314@163.com
 *   Description: ---
 *        Create: 2017-05-19 17:10:43
 * Last Modified: 2017-06-02 16:54:06
 */

#ifndef CH_MMAP_PACKETS_FORMAT_H
#define CH_MMAP_PACKETS_FORMAT_H

typedef struct ch_mmap_packets_format_t ch_mmap_packets_format_t;
typedef struct ch_mmap_packets_header_t ch_mmap_packets_header_t;
typedef struct ch_mmap_packets_entry_t  ch_mmap_packets_entry_t;
typedef struct ch_mmap_packets_iterator_t ch_mmap_packets_iterator_t;

#include <apr_pools.h>
#include "ch_mmap.h"
#include "ch_bin_format.h"

#define PFIMAGIC 0XFFFC
#define PEIMAGIC 0XFFFD
#define PMAJOR_VERSION  1
#define PMINJOR_VERSION 0

#define PROTO_TCP 0
#define PROTO_UDP 1

struct ch_mmap_packets_header_t {
    ch_mmap_buf_t mmb;
    ch_bin_format_t bfmt;
    uint32_t magic;
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t entries_number;
    uint32_t entries_size;
};
#define CH_MMAP_PACKETS_HEADER_LEN (sizeof(uint32_t)*5)

struct ch_mmap_packets_format_t {
    ch_mmap_packets_header_t cur_pheader;
    apr_pool_t *mp;
    ch_mmap_t *mm;
    int has_pheader;
    uint64_t entry_size;
};

struct ch_mmap_packets_entry_t {
    uint32_t magic;
    uint32_t entry_size;

    
	uint8_t  proto_id;
	uint8_t  flags;
	uint32_t sent_seq;
	uint32_t recv_ack;

    uint64_t time;
    
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;

	uint32_t p_size;
    size_t data_len;
    void *data;
};

#define CH_MMAP_PACKETS_ENTRY_HEADER_LEN (sizeof(uint8_t)*2+sizeof(uint16_t)*2+sizeof(uint32_t)*7+sizeof(uint64_t)+sizeof(size_t))

static inline uint32_t ch_mmap_packets_entry_len(ch_mmap_packets_entry_t *pentry){

    return pentry->data_len+ CH_MMAP_PACKETS_ENTRY_HEADER_LEN;
}

struct ch_mmap_packets_iterator_t {

    ch_mmap_packets_header_t *pheader;
    ch_mmap_packets_entry_t * (*next)(ch_mmap_packets_iterator_t *iter);
    uint32_t entries_count;
};

extern ch_mmap_packets_format_t * 
ch_mmap_packets_format_create(apr_pool_t *mp,const char *fname,uint64_t fsize,uint64_t entry_size,int is_write);

extern void ch_mmap_packets_format_destroy(ch_mmap_packets_format_t *mmfmt);

extern int ch_mmap_packets_format_put(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_entry_t *pentry);

extern void ch_mmap_packets_format_flush(ch_mmap_packets_format_t *mmfmt);

extern ch_mmap_packets_iterator_t *ch_mmap_packets_format_iterator_prefare(ch_mmap_packets_format_t *mmfmt);

extern void ch_mmap_packets_format_iterator_commit(ch_mmap_packets_format_t *mmfmt,ch_mmap_packets_iterator_t *iter);

extern void ch_mmap_packets_entry_dump(ch_mmap_packets_entry_t *pentry,FILE *out);

#endif /*CH_MMAP_PACKETS_FORMAT_H*/
