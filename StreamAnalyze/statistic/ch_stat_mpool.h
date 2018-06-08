/*
 *
 *      Filename: ch_stat_mpool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-06-05 10:51:27
 * Last Modified: 2018-06-05 10:51:27
 */

#ifndef CH_STAT_MPOOL_H
#define CH_STAT_MPOOL_H

typedef struct ch_stat_mpool_t ch_stat_mpool_t;

#include <stdlib.h>
#include <stdint.h>

struct ch_stat_mpool_t {

	int fd;
	void *start;
	void *pos;
	void *end;
};

#define ch_stat_mpool_pos(st_mp) ((st_mp)->pos)
#define ch_stat_mpool_pos_set(st_mp,npos) ((st_mp)->pos = npos)

#define ch_stat_mpool_size(st_mp) ((st_mp)->end-(st_mp)->start)

#define ch_stat_mpool_update(st_mp,sz) do {		\
	if((st_mp)->pos+(sz)>(st_mp)->end)			\
		(st_mp)->pos = (st_mp)->end;			\
	else										\
		(st_mp)->pos = (st_mp)->pos+(sz);		\
}while(0)

#define ch_stat_mpool_full(st_mp,sz) ((st_mp)->pos+(sz)>(st_mp)->end)


extern int ch_stat_mpool_init(ch_stat_mpool_t *st_mpool,const char *mmap_fname,size_t msize,int existed);

extern void ch_stat_mpool_fin(ch_stat_mpool_t *st_mpool);

extern void * ch_stat_mpool_alloc(ch_stat_mpool_t *st_mpool,size_t sz);

#endif /*CH_STAT_MPOOL_H*/
