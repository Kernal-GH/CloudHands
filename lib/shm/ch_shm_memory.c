/*
 * =====================================================================================
 *
 *       Filename:  ch_shm_memory.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2017 11:37:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <sys/shm.h>
#include "ch_log.h"
#include "ch_shm_memory.h"
#include "ch_constants.h"

static void * _memory_address_get(ch_shm_interface_t *shm_int,uint64_t offset){

   ch_shm_memory_t *shm_mem = (ch_shm_memory_t*)shm_int;

   return shm_mem->start_address+offset;
}

static void _memory_address_free(ch_shm_interface_t *shm_int,void *address){

    /*do nothing*/
}

/*del existed share memory*/
static int _del_existed_shm(key_t shm_key)
{
   struct shmid_ds buf,*buf_ptr=&buf;
   int id = shmget(shm_key,0,0);
   if (id == -1) {
      return 0;
   }
   memset(buf_ptr,0,sizeof(struct shmid_ds));
   if(shmctl(id,IPC_STAT,buf_ptr)==0)
   {
      if(buf_ptr->shm_nattch==0)
      {
         shmctl(id,IPC_RMID,NULL);
         return 0;
      }
   }

   ch_log(CH_LOG_ERR,"Cannot remove existed share memory!");

   return -1;
}

/*create a new share memory to write */
static int _create_shm_memory(ch_shm_memory_t *shm_mem,uint64_t size)
{
    int  id;
    key_t shm_key;
    /*get key*/
    shm_key=ftok((const char*)shm_mem->key,shm_mem->proj_id);
    
    if(shm_key==-1)
    {
	ch_log(CH_LOG_ERR,"Cannot create a share memory key:%s",shm_mem->key);
	return -1;
    }

    if(_del_existed_shm(shm_key)==-1)
    {
	return -1;
    }

    id = shmget(shm_key,size, (SHM_R|SHM_W|IPC_CREAT|IPC_EXCL));

    if (id == -1) {
        ch_log(CH_LOG_ERR,"Cannot create a share memory for key:%s",shm_mem->key);
        return -1;
    }

    shm_mem->start_address =shmat(id, NULL, 0);
    
    if (shm_mem->start_address == (void *) -1) 
    {
        ch_log(CH_LOG_ERR,"Cannot attach a share memory for key:%s",shm_mem->key);
	return -1;
    }

   shm_mem->shm_id = id;

    return 0;
}
/* open a existed share memory to read*/
static int _open_shm_memory(ch_shm_memory_t *shm_mem)
{
    int  id;
    key_t shm_key;
    /*get key*/
    shm_key=ftok(shm_mem->key,shm_mem->proj_id);
    
    if(shm_key==-1)
    {
       return -1;
    }

    id = shmget(shm_key,0,0);
    if (id == -1) {
    	ch_log(CH_LOG_ERR,"Cannot open share memory to read ,key:%s",shm_mem->key);
        return -1;
    }
    
    shm_mem->start_address = shmat(id, NULL, 0);

    if (shm_mem->start_address == (void *) -1) 
    {
	return -1;
    }

    shm_mem->shm_id=id;

    return 0;
}

ch_shm_interface_t * ch_shm_memory_create(ch_pool_t *mp,const char *key,int proj_id,uint64_t size,uint64_t  entry_size,uint64_t priv_data_size,int is_write){

   void *address = NULL;
   ch_shm_memory_t *shm_mem = NULL;

   uint64_t pg_size = sysconf(_SC_PAGE_SIZE); 
   uint64_t r_size =  0;
   uint64_t r_entry_size = 0;
   uint64_t r_priv_data_size = 0;
   int rc;

  r_size = (uint64_t)ch_align_up(size,pg_size);
  r_entry_size = (uint64_t)ch_align_up(entry_size,pg_size);

  if(priv_data_size!=0)
     r_priv_data_size = (uint64_t)ch_align_up(priv_data_size,pg_size);

  shm_mem = (ch_shm_memory_t*)ch_palloc(mp,sizeof(ch_shm_memory_t));
  shm_mem->key = key;
  shm_mem->proj_id = proj_id;

  if(is_write){
     rc = _create_shm_memory(shm_mem,r_size);
     if(rc <0){
        ch_log(CH_LOG_ERR,"Cannot create a share memory to write for key:%s!",key);
        return NULL;
     }
  }else{
     rc = _open_shm_memory(shm_mem);
     if(rc <0){
        ch_log(CH_LOG_ERR,"Cannot open share memory to read for key:%s!",key);
        return NULL;
      }
  }

  shm_mem->shm_int.mp = mp;
  shm_mem->shm_int.shm_address_get = _memory_address_get;
  shm_mem->shm_int.shm_address_free = _memory_address_free;

  ch_shm_init((ch_shm_interface_t*)shm_mem,shm_mem->start_address,
    r_size,r_entry_size,r_priv_data_size,pg_size,is_write);

  /*ok*/
  return (ch_shm_interface_t*)shm_mem;

}

void ch_shm_memory_destroy(ch_shm_interface_t *shm_int){

   ch_shm_memory_t *shm_mem = (ch_shm_memory_t*)shm_int;
   key_t shm_key;
   struct shmid_ds buf,*buf_ptr=&buf;
   shm_key = ftok((const char*)shm_mem->key,shm_mem->proj_id);
   if (-1 == shm_key)
      return;
   
   int id = shmget(shm_key, 0, 0);
   if (-1 == id)
      return;

   if (-1 == shmdt(shm_mem->start_address))
      return;
   
   memset(buf_ptr, 0, sizeof(struct shmid_ds));
   if(shmctl(id,IPC_STAT,buf_ptr)==0)
   {
      if(buf_ptr->shm_nattch==0)
      {
         shmctl(id,IPC_RMID,NULL);
      }
   }	
}

