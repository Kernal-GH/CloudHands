/*
 *
 *      Filename: ch_wb_memory.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-03 16:05:05
 * Last Modified: 2018-09-05 16:57:28
 */

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <sys/stat.h>

#include "ch_wb_memory.h"

#define F_IS_EXISTED(fname) (access(fname,F_OK) == 0)

static int _mmap_file_open(const char *fname,size_t fsize,int existed) {
   
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

static size_t  _get_fsize(const char *mmap_fname){

	 struct stat statbuf;  
	 stat(mmap_fname,&statbuf);  
	 
	 return statbuf.st_size;  
}

static void * _mmap_file_attach(int fd,size_t r_msize){

	return  mmap(NULL,r_msize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

}

int ch_wb_memory_init(ch_wb_memory_t *wbm,const char *mmap_fname,size_t msize){

	int existed = 0;
	size_t r_msize;
	size_t pg_size = sysconf(_SC_PAGE_SIZE); 
	size_t fsize = 0;

	/*align to pg_size*/
	r_msize =(((msize)+(pg_size-1))&~(pg_size-1));

	if(mmap_fname == NULL){
	
		fprintf(stdout,"Please specify the mmap file path!\n");
		return -1;
	}
	
	existed = F_IS_EXISTED(mmap_fname);

	if(!existed&&msize==0){
	
		fprintf(stdout,"Please specify the mmap size,mmapFile:%s\n",mmap_fname);
		return -1;
	}

	if(existed){
	
		fsize = _get_fsize(mmap_fname);
		if(fsize == 0){
		
			fprintf(stdout,"mmap filesize is zero!\n");
			return -1;
		}
		r_msize = fsize;
	}


	wbm->fd = _mmap_file_open(mmap_fname,r_msize,existed);
	if(wbm->fd <0){
	
		fprintf(stdout,"Cannot open file:%s ",mmap_fname);
		return -1;
	}

	wbm->start = _mmap_file_attach(wbm->fd,r_msize);
	if(wbm->start == NULL){

		fprintf(stdout,"cannot load mmapFile into memory:%s\n",mmap_fname);
		close(wbm->fd);
		return -1;
	}

	wbm->end = wbm->start+r_msize;
	wbm->pos = wbm->start;

	/*ok*/
	return 0;

}

void ch_wb_memory_fin(ch_wb_memory_t *wbm){

	close(wbm->fd);
}

void *ch_wb_memory_alloc(ch_wb_memory_t *wbm,size_t size){

	void *addr;

	if(CH_WB_MEMORY_FULL(wbm,size))
		return NULL;

	addr = wbm->pos;
	CH_WB_MEMORY_UPDATE(wbm,size);

	return addr;
}


void *ch_wb_memory_calloc(ch_wb_memory_t *wbm,size_t size){

	void *addr = ch_wb_memory_alloc(wbm,size);
	if(addr == NULL)
		return NULL;

	memset(addr,0,size);

	return addr;
}


