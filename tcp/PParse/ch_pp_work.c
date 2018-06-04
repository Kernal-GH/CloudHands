/*
 *
 *      Filename: ch_pp_work.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-17 18:06:50
 * Last Modified: 2018-05-17 19:18:40
 */

#include "ch_pp_work.h"
#include "ch_log.h"
#include "ch_tcp_record.h"

static int is_exit = 0;

ch_pp_work_t * ch_pp_work_create(ch_pool_t *mp,const char *cfname,const char *shm_fname){


	ch_pp_work_t *pwork = NULL;

	pwork = (ch_pp_work_t*)ch_palloc(mp,sizeof(*pwork));

	pwork->mp = mp;
	pwork->pcontext = ch_pp_context_create(mp,cfname);
	if(pwork->pcontext == NULL){
	
		return NULL;
	}
    
	/*init log*/
    ch_log_init(mp,pwork->pcontext->log_file,pwork->pcontext->log_level);


	pwork->pp_pool = ch_pp_pool_create(pwork);
	if(pwork->pp_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create proto parser pool failed!");
		return NULL;
	}
	pwork->shm_fmt = ch_shm_format_tcp_with_mmap_create(mp,shm_fname,0,0,0,0);
	
	if(pwork->shm_fmt == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot open the tcp record shm format to read:%s",shm_fname);
		return NULL;
	}


	
	return pwork;
}

int ch_pp_work_start(ch_pp_work_t *pwork){

	ch_shm_format_t *shm_fmt = pwork->shm_fmt;
	ch_pp_pool_t *pp_pool = pwork->pp_pool;

	ch_shm_entry_iterator_t *iter = NULL;
	ch_tcp_record_t *tcp_record;
	
	while(!is_exit){
	
		iter = ch_shm_entry_iterator_prefare(shm_fmt);
		if(iter == NULL){
		
			ch_log(CH_LOG_INFO,"%s: iter is null, sleep 100000 useconds.", __func__);
			
			usleep(100000); /*100ms*/
			continue;
		}

		while((tcp_record = (ch_tcp_record_t*)iter->next(iter))){

			ch_pp_session_parse(pp_pool,tcp_record);
		}

		ch_shm_entry_iterator_commit(shm_fmt,iter);
	}

	/*exit*/
	is_exit = 0;

	ch_log(CH_LOG_WARN,"Parser work will exit---------------------------------");

	if (shm_fmt && iter)
		ch_shm_entry_iterator_commit(shm_fmt, iter);

	return 0;
}

void ch_pp_work_stop(ch_pp_work_t *pwork ch_unused){

	is_exit = 1;

}



