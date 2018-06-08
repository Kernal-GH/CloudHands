/*
 *
 *      Filename: ch_stat_mpool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-05 11:20:21
 * Last Modified: 2018-06-08 12:08:59
 */

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "ch_constants.h"
#include "ch_stat_mpool.h"

static int _mmap_file_open(const char *fname,uint64_t fsize,int existed){
   int fd;

   if(existed){
	   
	   return open(fname,O_RDWR,0644);
   }

   fd = open(fname,O_RDWR|O_CREAT,0644);
   if(fd <0){
      return fd;
   }
   /*alloc space file*/
   if(fallocate(fd,0,0,fsize)){
      return -1;
   }
   /*ok*/
   return fd;
}

static int _mmap_file_attach(ch_stat_mpool_t *st_mpool,uint64_t r_msize){

	st_mpool->start = mmap(NULL,r_msize,PROT_READ|PROT_WRITE,MAP_SHARED,st_mpool->fd,0);
	if(st_mpool->start == NULL)
		return -1;

	st_mpool->pos = st_mpool->start;
	st_mpool->end = st_mpool->start+r_msize;

	return 0;
}

int ch_stat_mpool_init(ch_stat_mpool_t *st_mpool,const char *mmap_fname,size_t msize,int existed){

	int fd;
	uint64_t r_msize;
	uint64_t pg_size = sysconf(_SC_PAGE_SIZE); 

	r_msize = (uint64_t)ch_align_up(msize,pg_size);

	fd = _mmap_file_open(mmap_fname,r_msize,existed);

	if(fd <0){
	
		fprintf(stderr,"Cannot open file:%s ",mmap_fname);
		return -1;
	}

	st_mpool->fd = fd;

	if(_mmap_file_attach(st_mpool,r_msize)){
	
		fprintf(stderr,"Cannot mmap file:%s",mmap_fname);
		return -1;
	}


	return 0;
}

void ch_stat_mpool_fin(ch_stat_mpool_t *st_mpool){

	munmap(st_mpool->start,ch_stat_mpool_size(st_mpool));
	close(st_mpool->fd);

}

void * ch_stat_mpool_alloc(ch_stat_mpool_t *st_mpool,size_t sz){

	void *addr;

	if(ch_stat_mpool_full(st_mpool,sz))
		return NULL;

	addr = st_mpool->pos;
	
	ch_stat_mpool_update(st_mpool,sz);

	return addr;
}


