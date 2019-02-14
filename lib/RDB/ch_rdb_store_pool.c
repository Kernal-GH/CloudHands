/*
 *
 *      Filename: ch_rdb_store_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-04-15 13:32:56
 * Last Modified: 2018-04-21 12:42:15
 */

#include "ch_rdb_store_pool.h"
#include "ch_log.h"

static uint64_t TYPES_2_TV[BY_TIME_N] = {1l,60l,3600l,86400l,604800l};

#define is_new_rdb_need_create(rdb_pool,cur_time) (cur_time-rdb_pool->last_rdb_create_time>TYPES_2_TV[rdb_pool->rdbname_create_type])||(rdb_pool->cur_rdb_store == NULL)

ch_rdb_store_pool_t *
ch_rdb_store_pool_create(ch_pool_t *mp,const char *rdb_store_dir,const char *rdb_name_prefix,const char *create_type,uint64_t rdb_using_timeout){

	struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t cur_time = tv.tv_sec;

	ch_rdb_store_pool_t *rdb_pool = NULL;

	rdb_pool = (ch_rdb_store_pool_t*)ch_palloc(mp,sizeof(*rdb_pool));

	rdb_pool->mp = mp;

	INIT_LIST_HEAD(&rdb_pool->rdb_stores);

	rdb_pool->rdb_store_dir = rdb_store_dir;
	rdb_pool->rdb_name_prefix = rdb_name_prefix;
	rdb_pool->rdbname_create_type = ch_rdbname_create_type_get(create_type);

	rdb_pool->last_rdb_create_time = 0;
	rdb_pool->cur_rdb_store = NULL;
	rdb_pool->rdb_store_n = 0;
	rdb_pool->rdb_name_len = strlen(rdb_store_dir)+1+strlen(rdb_name_prefix)+1+13+3;
	rdb_pool->rdb_last_check_time = cur_time;
	rdb_pool->rdb_using_timeout = rdb_using_timeout;

	return rdb_pool;
}

void ch_rdb_store_pool_close(ch_rdb_store_pool_t *rdb_pool){

	ch_rdb_store_t *rdb_store = NULL;

	list_for_each_entry(rdb_store,&rdb_pool->rdb_stores,node){
	
		ch_rdb_store_close(rdb_store);

	}

	if(rdb_pool->cur_rdb_store){
	
		ch_rdb_store_close(rdb_pool->cur_rdb_store);
		rdb_pool->cur_rdb_store = NULL;

	}

}

static inline char* _rdb_name_get(ch_rdb_store_pool_t *rdb_pool,uint64_t time){


	char *name = (char*)calloc(1,rdb_pool->rdb_name_len);
	if(name == NULL)
		return NULL;

	snprintf(name,rdb_pool->rdb_name_len,"%s%s_%lu",rdb_pool->rdb_store_dir,rdb_pool->rdb_name_prefix,time);

	return name;
}

ch_rdb_store_t* ch_rdb_store_pool_get(ch_rdb_store_pool_t *rdb_pool){


	ch_rdb_store_t *rdb = NULL;
    
	struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t cur_time = tv.tv_sec;
	char *rdb_name;


	if(is_new_rdb_need_create(rdb_pool,cur_time)){
	
		rdb_name = _rdb_name_get(rdb_pool,cur_time);
		if(rdb_name == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot Get The RDB Store Name!");
			return NULL;
		}

		rdb = ch_rdb_store_create(rdb_pool->mp,rdb_name);
		if(rdb == NULL){
		
			ch_log(CH_LOG_ERR,"Create RDB Store:%s failed!",rdb_name);
			free(rdb_name);
			return NULL;
		}

		if(rdb_pool->cur_rdb_store){

			list_add_tail(&rdb_pool->cur_rdb_store->node,&rdb_pool->rdb_stores);
		}
		
		rdb_pool->rdb_store_n += 1;
		rdb_pool->cur_rdb_store = rdb;
		rdb_pool->last_rdb_create_time = cur_time;

	}else{
	
		rdb = rdb_pool->cur_rdb_store;
	}

	ch_rdb_store_ref_update(rdb,1);

	return rdb;
}

void ch_rdb_store_pool_check(ch_rdb_store_pool_t *rdb_pool){

	struct timeval tv;
    gettimeofday(&tv,NULL);
    uint64_t cur_time = tv.tv_sec;

	ch_rdb_store_t *rdb = NULL,*rdb_tmp = NULL;
	
	
	if(cur_time-rdb_pool->rdb_last_check_time<rdb_pool->rdb_using_timeout){
	
		return;
	}

	list_for_each_entry_safe(rdb,rdb_tmp,&rdb_pool->rdb_stores,node){
	

		if(ch_rdb_store_is_no_ref(rdb)||ch_rdb_store_is_timeout(rdb,cur_time,rdb_pool->rdb_using_timeout)){
			
			ch_log(CH_LOG_DEBUG,"The RDB Store:%s has no been refer or timeout!",rdb->rdb_name);
		
			ch_rdb_store_close(rdb);
			list_del(&rdb->node);
			
			rdb_pool->rdb_store_n -=1;


		}
	}

	rdb_pool->rdb_last_check_time = cur_time;

}

void ch_rdb_store_pool_dump(ch_rdb_store_pool_t *rdb_pool,FILE *out){

	ch_rdb_store_t *rdb_store = NULL;

	fprintf(out,"Dump The RDB Pool Informations:\n");
	fprintf(out,"RDB Store Root Dir:%s\n",rdb_pool->rdb_store_dir);
	fprintf(out,"RDB Store DBName Prefix:%s\n",rdb_pool->rdb_name_prefix);
	fprintf(out,"RDB Store number:%d\n",(int)rdb_pool->rdb_store_n);
	
	fprintf(out,"Dump The Current RDB Store Informations:\n");
	if(rdb_pool->cur_rdb_store)
		ch_rdb_store_dump(rdb_pool->cur_rdb_store,out);  

	fprintf(out,"Dump The Old RDB Store Informations:\n");
	
	list_for_each_entry(rdb_store,&rdb_pool->rdb_stores,node){
	
		
		ch_rdb_store_dump(rdb_store,out);
	}

}

