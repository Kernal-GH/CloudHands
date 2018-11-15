/*
 *
 *      Filename: ch_rdb_store_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-13 18:10:34
 * Last Modified: 2018-04-13 18:10:34
 */

#ifndef CH_RDB_STORE_POOL_H
#define CH_RDB_STORE_POOL_H

typedef struct ch_rdb_store_pool_t ch_rdb_store_pool_t;

#include "ch_mpool.h"
#include "ch_rdb_store.h"
#include "ch_list.h"

typedef enum {

    BY_SEC = 0,
    BY_MIN,
    BY_HOUR,
    BY_DAY,
	BY_WEEK,
	BY_TIME_N
}rdbname_create_by_time_e;


struct ch_rdb_store_pool_t {

	struct list_head rdb_stores;

	ch_pool_t *mp;

	const char *rdb_store_dir;

	const char *rdb_name_prefix;

	int rdbname_create_type;

	uint64_t rdb_using_timeout;

	uint64_t rdb_last_check_time;

	uint64_t last_rdb_create_time;

	ch_rdb_store_t *cur_rdb_store;

	int rdb_store_n;

	size_t rdb_name_len;
};

static inline int ch_rdbname_create_type_get(const char *v){

	if(*v=='s'||*v=='S')
		return BY_SEC;
	
	if(*v=='m'||*v=='M')
		return BY_MIN;

	if(*v=='h'||*v=='H')
		return BY_HOUR;

	if(*v=='d'||*v=='D')
		return BY_DAY;

	if(*v=='w'||*v=='W')
		return BY_WEEK;
	
	return -1;
}



extern ch_rdb_store_pool_t *
ch_rdb_store_pool_create(ch_pool_t *mp,const char *rdb_store_dir,const char *rdb_name_prefix,const char *create_type,uint64_t rdb_using_timeout);

extern void ch_rdb_store_pool_close(ch_rdb_store_pool_t *rdb_pool);

extern ch_rdb_store_t* ch_rdb_store_pool_get(ch_rdb_store_pool_t *rdb_pool);

extern void ch_rdb_store_pool_check(ch_rdb_store_pool_t *rdb_pool);

extern void ch_rdb_store_pool_dump(ch_rdb_store_pool_t *rdb_pool,FILE *out);

#endif /*CH_RDB_STORE_POOL_H*/
