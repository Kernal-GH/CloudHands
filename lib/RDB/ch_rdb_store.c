/*
 *
 *      Filename: ch_rdb_store.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-27 12:08:29
 * Last Modified: 2018-04-21 10:39:17
 */

#include "ch_rdb_store.h"
#include "ch_log.h"

ch_rdb_store_t * 
ch_rdb_store_create(ch_pool_t *mp,char *rdb_name){

	char *err = NULL;
	long cpus;

	ch_rdb_store_t *rdb_store = NULL;

	rdb_store = (ch_rdb_store_t*)ch_pcalloc(mp,sizeof(*rdb_store));

	rdb_store->rdb_name = rdb_name;

	rdb_store->ok_writen_num = 0;
	rdb_store->ok_writen_bytes = 0;
	rdb_store->err_writen_num = 0;
	rdb_store->err_writen_bytes = 0;

	rdb_store->rks_db_options = rocksdb_options_create();
	if(rdb_store->rks_db_options == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create rocks db options!");
		return NULL;
	}

    /*cpus = sysconf(_SC_NPROCESSORS_ONLN);  // get # of online cores
	rocksdb_options_increase_parallelism(rdb_store->rks_db_options, (int)(cpus));*/
	rocksdb_options_optimize_level_style_compaction(rdb_store->rks_db_options, 0);
	rocksdb_options_set_create_if_missing(rdb_store->rks_db_options, 1);

	rdb_store->rks_db = rocksdb_open(rdb_store->rks_db_options, rdb_store->rdb_name, &err);

	if(rdb_store->rks_db == NULL || err){
	
		ch_log(CH_LOG_ERR,"Create rocks db failed:%s,%s",rdb_store->rdb_name,err);
		return NULL;
	}

	rdb_store->rks_db_write_options = rocksdb_writeoptions_create();
	if(rdb_store->rks_db_write_options == NULL){
	
		ch_log(CH_LOG_ERR,"Create rocks db write options failed!");

		return NULL;
	}

	rdb_store->ref_count = 0;

	return rdb_store;
}

void 
ch_rdb_store_close(ch_rdb_store_t *rdb_store){


	rocksdb_writeoptions_destroy(rdb_store->rks_db_write_options);
	rocksdb_options_destroy(rdb_store->rks_db_options);
	rocksdb_close(rdb_store->rks_db);
	if(rdb_store->rdb_name)
	{
	
		free(rdb_store->rdb_name);
		rdb_store->rdb_name = NULL;
	}


}

int
ch_rdb_store_put(ch_rdb_store_t *rdb_store,const char *key,size_t klen,const char *val,size_t vlen){

	char *err = NULL;
	struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t cur_time = tv.tv_sec;

	rdb_store->last_write_time = cur_time;

	rocksdb_put(rdb_store->rks_db,rdb_store->rks_db_write_options,key,klen,val,vlen,&err);
	
	if(err){
	
		ch_log(CH_LOG_ERR,"RocksDB write key:%s failed,error:%s!",key,err);
		
		rdb_store->err_writen_bytes += vlen;
		rdb_store->err_writen_num += 1;

		return -1;

	}

	rdb_store->ok_writen_num += 1;
	rdb_store->ok_writen_bytes += vlen;

	return 0;
}

void ch_rdb_store_dump(ch_rdb_store_t *rdb_store,FILE *out){


	fprintf(out,"Dump the rocksdb info:\n");
	fprintf(out,"RocksDB store the path:%s\n",rdb_store->rdb_name);
	fprintf(out,"RocksDB ok writen number:%lu\n",(unsigned long)rdb_store->ok_writen_num);
	fprintf(out,"RocksDB ok writen bytes:%lu\n",(unsigned long)rdb_store->ok_writen_bytes);
	fprintf(out,"RocksDB err writen number:%lu\n",(unsigned long)rdb_store->err_writen_num);
	fprintf(out,"RocksDB err writen bytes:%lu\n",(unsigned long)rdb_store->err_writen_bytes);

	fprintf(out,"RocksDB ref count:%lu\n",(unsigned long)rdb_store->ref_count);

}

