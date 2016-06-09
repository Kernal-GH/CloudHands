/*************************************************************************
 > File Name: ch_mmap.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月08日 星期三 16时24分10秒
 ************************************************************************/
#include <sys/mman.h>
#include "ch_constants.h"
#include "ch_log.h"
#include "ch_mmap.h"
#include "ch_file.h"

static inline ch_mmap_header_t * _mmap_header_attach(ch_mmap_t *mm){

	ch_mmap_header_t *mmh;
	void *addr = mmap(NULL,sizeof(ch_mmap_header_t),PROT_READ|PROT_WRITE,MAP_SHARED,mm->fd,0);

	if(addr == MAP_FAILED){
		return NULL;
	}

	return (ch_mmap_header_t*)addr;
}

static inline int _is_file_existed(const char *fname){

	return access(fname,F_OK) == 0;
}

static inline int  _mmap_file_open_for_read(const char *fname){

	return ch_file_open(fname,CH_FILE_RDWR,CH_FILE_OPEN,CH_FILE_DEFAULT_ACCESS);
}



static inline int _mmap_file_open_for_write(const char *fname,int existed,uint64_t fsize){

	int fd;
	
	fd = ch_file_open(fname,CH_FILE_RDWR,CH_FILE_CREATE_OR_OPEN,CH_FILE_DEFAULT_ACCESS);

	if(fd <0 || existed){
		return fd;
	}
	/*alloc space file*/
	if(fallocate(fd,0,0,fsize)){
		return -1;
	}

	/*ok*/
	return fd;
}

static inline uint64_t _mmap_entries_count_get(ch_mmap_t *mm,uint64_t hsize,uint64_t entry_size){

	return ((uint64_t)(mm->fsize)-hsize)/entry_size;
} 

static void  _mmap_header_init(ch_mmap_t *mm,uint64_t entry_size){

	ch_mmap_header_t *mh = mm->mmap_header;
	uint64_t hsize = (uint64_t)ch_align_up(sizeof(ch_mmap_header_t),mm->page_size);

	mh->mmap_entries_start = hsize;
	mh->mmap_entries_count = _mmap_entries_count_get(mm,hsize,entry_size);
	mh->mmap_entry_size = entry_size;
	mh->mmap_write_entry_pos = mh->mmap_entries_start;
	mh->mmap_read_entry_pos = mh->mmap_entries_start;
}

ch_mmap_t * ch_mmap_create(apr_pool_t *mp,const char *fname,uint64_t fsize,uint64_t  entry_size,int is_write){

	ch_mmap_t *mm = NULL;
	uint64_t pg_size = sysconf(_SC_PAGE_SIZE); 
	uint64_t r_fsize =  0;
	uint64_t r_entry_size = 0;
	int fd;
	int existed = _is_file_existed(fname);

	if(existed == 0 && is_write == 0){

		ch_log(CH_LOG_ERR,"File:%s not existed,cannot open for mmap and read!",fname);
		return NULL;
	}

	r_fsize = (uint64_t)ch_align_up(fsize,pg_size);
	r_entry_size = (uint64_t)ch_align_up(entry_size,pg_size);

	if(is_write){
		fd = _mmap_file_open_for_write(fname,existed,r_fsize);
		if(fd <0){
			ch_log(CH_LOG_ERR,"Cannot open file:%s write by mmap!",fname);
			return NULL;
		}
	}else{
		fd = _mmap_file_open_for_read(fname);
		if(fd <0){
			ch_log(CH_LOG_ERR,"Cannot open file:%s read by mmap!",fname);
			return NULL;
		}
	}

	mm = (ch_mmap_t *)apr_palloc(mp,sizeof(ch_mmap_t));
	mm->page_size = pg_size;
	mm->mp = mp;
	mm->fname = fname;
	mm->fd = fd;
	mm->fsize = r_fsize;
	mm->is_write = is_write?1:0;

	mm->mmap_header = _mmap_header_attach(mm);
	if(mm->mmap_header == NULL){

		ch_log(CH_LOG_ERR,"Cannot mmap header!");
		close(fd);
		return NULL;
	}
	if(existed == 0){
		_mmap_header_init(mm,r_entry_size);
	}
	/*ok*/
	return mm;
}

void ch_mmap_destroy(ch_mmap_t *mm){

	if(mm->is_write == 0){
		ch_log(CH_LOG_ERR,"Cao,You cannot do it!");
		
	}else{

		close(mm->fd);
		ch_file_delete(mm->fname);
	}
}
