/*
 * =====================================================================================
 *
 *       Filename:  ch_shm_memory.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2017 10:50:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SHM_MEMORY_H
#define CH_SHM_MEMORY_H

typedef struct ch_shm_memory_t ch_shm_memory_t;

#include "ch_shm_interface.h"

struct ch_shm_memory_t {

   ch_shm_interface_t shm_int;

   void *start_address;

   const char *key;
   int shm_id;
   int proj_id;
};

ch_shm_interface_t * ch_shm_memory_create(ch_pool_t *mp,const char *key,int proj_id,uint64_t size,uint64_t  entry_size,uint64_t priv_data_size,int is_write);


extern void ch_shm_memory_destroy(ch_shm_interface_t *shm_int);

#endif /* CH_SHM_MEMORY_H */

