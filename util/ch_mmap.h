/*************************************************************************
 > File Name: ch_mmap.h
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月08日 星期三 10时25分29秒
 ************************************************************************/

#ifndef CH_MMAP_H
#define CH_MMAP_H

/*Before declaration */
typedef struct ch_mmap_t ch_mmap_t;
typedef struct ch_mmap_header_t ch_mmap_header_t;
typedef struct ch_mmap_buf_t ch_mmap_buf_t;

#include <apr_pools.h>

struct ch_mmap_t {

	uint64_t page_size;
	/*The memory pool used to allocate the instance of ch_mmap_t */
	apr_pool_t *mp;

	/*The file name used to mmap*/
	const char *fname;

	/*file decs used to mmap*/
	int fd; 

	/*The size of file used to mmap*/
	uint64_t fsize;

	/*whether used to write*/
	unsigned is_write:1;

	/*The instance of mmap header*/
	ch_mmap_header_t *mmap_header;
};

#pragma pack(push,1)
struct ch_mmap_header_t {
	uint64_t mmap_entries_start;
	uint64_t mmap_entries_count;
	uint64_t mmap_entry_size;
    uint64_t mmap_entries_count_cur;
	uint64_t mmap_write_entry_pos;
	uint64_t mmap_read_entry_pos;

};

#pragma pack(pop)

struct ch_mmap_buf_t {
	ch_mmap_t *mm;
	uint64_t entry_index;
	void *start;
	void *pos;
	void *end;
};

static inline uint64_t ch_mmap_entry_pos_offset(ch_mmap_t *mm,uint64_t index){

    return mm->mmap_header->mmap_entry_size*index+mm->mmap_header->mmap_entries_start;
}

static inline void ch_mmap_entry_pos_update(ch_mmap_t *mm,int pos_c,int c){

    uint64_t * pos = mm->is_write?&mm->mmap_header->mmap_write_entry_pos:&mm->mmap_header->mmap_read_entry_pos;
    uint64_t count = mm->mmap_header->mmap_entries_count;

    *pos = (*pos+pos_c)%count;
    mm->mmap_header->mmap_entries_count_cur += c;

}

static inline int ch_mmap_empty(ch_mmap_t *mm){
    return mm->mmap_header->mmap_entries_count_cur == 0;
}

static inline int ch_mmap_full(ch_mmap_t *mm){
    return mm->mmap_header->mmap_entries_count_cur == mm->mmap_header->mmap_entries_count;
}

extern ch_mmap_t * ch_mmap_create(apr_pool_t *mp,const char *fname,uint64_t fsize,uint64_t  entry_size,int is_write);

extern void ch_mmap_destroy(ch_mmap_t *mm);

extern int ch_mmap_buf_get(ch_mmap_t *mm,ch_mmap_buf_t *mmb);

extern void ch_mmap_buf_commit(ch_mmap_t *mm,ch_mmap_buf_t *mmb);

#endif /*CH_MMAP_H*/

