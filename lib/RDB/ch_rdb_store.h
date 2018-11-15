/*
 *
 *      Filename: ch_rdb_store.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-26 09:49:42
 * Last Modified: 2018-03-26 09:49:42
 */

#ifndef CH_RDB_STORE_H
#define CH_RDB_STORE_H

typedef struct ch_rdb_store_t ch_rdb_store_t;

#include <rocksdb/c.h>
#include "ch_list.h"
#include "ch_mpool.h"

struct ch_rdb_store_t {

	struct list_head node;

	rocksdb_t *rks_db;

	rocksdb_options_t *rks_db_options;

	rocksdb_writeoptions_t *rks_db_write_options;

	uint64_t last_write_time;

	uint64_t ok_writen_num;
	uint64_t ok_writen_bytes;
	uint64_t err_writen_num;
	uint64_t err_writen_bytes;

	uint64_t ref_count;

	char *rdb_name;
};

#define ch_rdb_store_ref_update(rdb,c) (rdb->ref_count=rdb->ref_count+c)
#define ch_rdb_store_is_no_ref(rdb) (rdb->ref_count == 0)
#define ch_rdb_store_is_timeout(rdb,time,tv) (time-rdb->last_write_time>tv) 

extern ch_rdb_store_t * 
ch_rdb_store_create(ch_pool_t *mp,char *rdb_name);

extern void 
ch_rdb_store_close(ch_rdb_store_t *rdb_store);

extern int
ch_rdb_store_put(ch_rdb_store_t *rdb_store,const char *key,size_t klen,const char *val,size_t vlen);

extern void ch_rdb_store_dump(ch_rdb_store_t *rdb_store,FILE *out);

#endif /*CH_RDB_STORE_H*/
