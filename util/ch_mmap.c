/*************************************************************************
 > File Name: ch_mmap.c
 > Author: shajf
 > Mail: csp001314@163.com 
 > Created Time: 2016年06月08日 星期三 16时24分10秒
 ************************************************************************/

#include "ch_log.h"
#include "ch_mmap.h"

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

	return open(fname,O_RDWR);
}



static inline int _mmap_file_open_for_write(const char *fname,int existed,size_t fsize){

	int fd;
	
	fd = open(fname,O_RDWR|O_CREAT);
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

static int  _mmap_header_init(ch_mmap_t *mm,uint32_t entry_size){

}

ch_mmap_t * ch_mmap_create(apr_pool_t *mp,const char *fname,size_t fsize,uint32_t  entry_size,int is_write){

	ch_mmap_t *mm = NULL;
	size_t pg_size = sysconf(_SC_PAGE_SIZE); 
	size_t r_fsize =  0;
	uint32_t r_entry_size = 0;
	int fd;
	int existed = _is_file_existed(fname);

	if(existed == 0 && is_write == 0){

		ch_log(CH_LOG_ERR,"File:%s not existed,cannot open for mmap and read!",fname);
		return NULL;
	}

	r_fsize = ch_align_up(fsize,pg_size);
	r_entry_size = (uint32_t)ch_align_up(entry_size,pg_size);

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
		if(_mmap_header_init(mm,r_entry_size)){

			ch_log(CH_LOG_ERR,"Init the mmap header failed!");
			return NULL;
		}
	}
	/*ok*/
	return mm;
}
