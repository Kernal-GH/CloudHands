/*
 * =====================================================================================
 *
 *       Filename:  ch_shm_mmap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/31/2017 06:50:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jacks), csp001314@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SHM_MMAP_H
#define CH_SHM_MMAP_H

typedef struct ch_shm_mmap_t ch_shm_mmap_t;

#include "ch_shm_interface.h"

struct ch_shm_mmap_t {

   ch_shm_interface_t shm_int;

   /*The file name used to mmap*/
   const char *fname;

   /*file decs used to mmap*/
   int fd; 

};

extern ch_shm_interface_t * ch_shm_mmap_create(ch_pool_t *mp,const char *fname,uint64_t fsize,uint64_t  entry_size,uint64_t priv_data_size,int is_write);


extern void ch_shm_mmap_destroy(ch_shm_interface_t *shm_int);

#endif /* CH_SHM_MMAP_H */

