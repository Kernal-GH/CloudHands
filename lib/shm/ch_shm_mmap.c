/*
 * =====================================================================================
 *
 *       Filename:  ch_shm_mmap.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2017 07:00:31 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sys/mman.h>
#include <fcntl.h>
#include "ch_log.h"
#include "ch_shm_mmap.h"
#include "ch_constants.h"

static void * _mmap_address_get(ch_shm_interface_t *shm_int,uint64_t offset){

    void *addr = NULL;
    ch_shm_mmap_t *shm_mmap = (ch_shm_mmap_t*)shm_int;

    if(shm_int->is_write){
        addr = mmap(NULL,shm_int->shm_header->shm_entry_size,PROT_READ|PROT_WRITE,MAP_SHARED,shm_mmap->fd,offset);
    }else{
        addr = mmap(NULL,shm_int->shm_header->shm_entry_size,PROT_READ,MAP_PRIVATE,shm_mmap->fd,offset);
    }

    return addr;
}

static void _mmap_address_free(ch_shm_interface_t *shm_int,void *address){

    munmap(address,shm_int->shm_header->shm_entry_size);
}

static inline int _is_file_existed(const char *fname){

   return access(fname,F_OK) == 0;
}

static inline int _mmap_file_open_for_write(const char *fname,int existed,uint64_t fsize){
   int fd;
   fd = open(fname,O_RDWR|O_CREAT,0644);
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

static inline int  _mmap_file_open_for_read(const char *fname){

   return open(fname,O_RDWR,0644);
}

static inline void * _mmap_header_attach(int fd,uint64_t page_size,uint64_t priv_data_size){

	size_t sz = ch_align_up(sizeof(ch_shm_header_t)+priv_data_size,page_size);
	void *addr = mmap(NULL,sz,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

	if(addr == MAP_FAILED){
		return NULL;
	}

	return addr;
}

ch_shm_interface_t * ch_shm_mmap_create(ch_pool_t *mp,const char *fname,uint64_t fsize,uint64_t  entry_size,uint64_t priv_data_size,int is_write){
   
   void *address = NULL;
   ch_shm_mmap_t *shm_mmap = NULL;
   uint64_t pg_size = sysconf(_SC_PAGE_SIZE); 
   uint64_t r_fsize =  0;
   uint64_t r_entry_size = 0;
   uint64_t r_priv_data_size = 0;
   int fd;
   int existed = _is_file_existed(fname);
   if(existed == 0 && is_write == 0){

          ch_log(CH_LOG_ERR,"File:%s not existed,cannot open for mmap and read!",fname);
          return NULL;
  }

  r_fsize = (uint64_t)ch_align_up(fsize,pg_size);
  r_entry_size = (uint64_t)ch_align_up(entry_size,pg_size);

  if(priv_data_size!=0)
     r_priv_data_size = (uint64_t)ch_align_up(priv_data_size,pg_size);

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

  shm_mmap = (ch_shm_mmap_t*)ch_palloc(mp,sizeof(ch_shm_mmap_t));
  shm_mmap->shm_int.mp = mp;
  shm_mmap->shm_int.shm_address_get = _mmap_address_get;
  shm_mmap->shm_int.shm_address_free = _mmap_address_free;
  shm_mmap->fname = fname;
  shm_mmap->fd = fd;

  address =_mmap_header_attach(fd,pg_size,r_priv_data_size);

  if(address == NULL){
     ch_log(CH_LOG_ERR,"Cannot attach mmap address!");
     close(fd);
     return NULL;
  }
 
  ch_shm_init((ch_shm_interface_t*)shm_mmap,address,
    r_fsize,r_entry_size,r_priv_data_size,pg_size,is_write);

  /*ok*/
  return (ch_shm_interface_t*)shm_mmap;

}

void ch_shm_mmap_destroy(ch_shm_interface_t *shm_int){

   ch_shm_mmap_t *shm_mmap = (ch_shm_mmap_t*)shm_int;
   
   munmap(shm_int->shm_header,shm_int->shm_header->shm_entries_start);

   if(shm_int->is_write == 0){
      ch_log(CH_LOG_ERR,"Cao,Reader process can not destroy shm!");
		
   }else{
      
      close(shm_mmap->fd);
      unlink(shm_mmap->fname);
   }
}

