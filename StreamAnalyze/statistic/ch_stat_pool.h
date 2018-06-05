/*
 *
 *      Filename: ch_stat_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-04 11:09:20
 * Last Modified: 2018-06-04 11:09:20
 */

#ifndef CH_STAT_POOL_H
#define CH_STAT_POOL_H

typedef struct ch_stat_pool_t ch_stat_pool_t;
typedef struct ch_stat_pool_hdr_t ch_stat_pool_hdr_t;

#include "ch_stat_mpool.h"
#include "ch_mpool.h"

/*Stat types*/
enum {

	STAT_ALL = 0,
	STAT_TCP,
	STAT_UDP,
	STAT_ICMP,
	STAT_ARP,
	STAT_OTHER,
	STAT_NUM,
};

#pragma pack(push,1)
struct ch_stat_pool_hdr_t {

	uint64_t base_time;
	uint64_t stat_time_up;
	uint64_t stat_time_tv;

};
#pragma pack(pop)

struct ch_stat_pool_t {

	ch_pool_t *mp;

	ch_stat_pool_hdr_t *p_hdr;
	
	ch_stat_mpool_t st_mpool;

	ch_stat_obj_t stat_objs[STAT_NUM];
};


extern ch_stat_pool_t * ch_stat_pool_create(ch_pool_t *mp,const char *mmap_fname,
	uint64_t stat_time_up,uint64_t stat_time_tv);

extern void ch_stat_pool_handle(ch_stat_pool_t *st_pool,uint64_t time,uint64_t pkt_size,int pkt_type);

extern void ch_stat_pool_update(ch_stat_pool_t *st_pool);

#endif /*CH_STAT_POOL_H*/
