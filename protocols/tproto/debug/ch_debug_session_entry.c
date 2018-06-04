/*
 *
 *      Filename: ch_debug_session_entry.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 22:39:24
 * Last Modified: 2017-05-31 19:10:56
 */

#include <apr_strings.h>
#include "ch_log.h"
#include "ch_debug_session_entry.h"

void ch_debug_sentry_init(ch_debug_session_entry_t *dsentry,
	ch_session_entry_t *sentry,ch_mmap_file_entry_t *fentry,apr_pool_t *mp){

	ch_session_entry_init(sentry,fentry);
	dsentry->mp = mp;
	dsentry->req_fp = NULL;
	dsentry->res_fp = NULL;
	dsentry->req_fpath = NULL;
	dsentry->res_fpath = NULL;

	sentry->priv_data = (void*)dsentry;
}

void ch_debug_sentry_data_write(ch_debug_session_entry_t *dsentry,void *data,size_t dlen,int is_req){


	FILE *fp = is_req?dsentry->req_fp:dsentry->res_fp;

	if(fp){
	
		fwrite(data,dlen,1,fp);
	}
}

int ch_debug_sentry_fpath_init(ch_debug_session_entry_t *dsentry,const char *fpath,int is_req){

	FILE *fp = fopen(fpath,"a+");
	if(fp == NULL){
	
		ch_log(CH_LOG_ERR,"cannot open file:%s to writing!",fpath);
		return -1;
	}

	if(is_req){
	
		dsentry->req_fp = fp;
		dsentry->req_fpath = apr_pstrdup(dsentry->mp,fpath);
	}else{
		dsentry->res_fp = fp;
		dsentry->res_fpath = apr_pstrdup(dsentry->mp,fpath);
	
	}

	return 0;
}

void ch_debug_session_entry_fin(ch_debug_session_entry_t *dsentry){

	if(dsentry->req_fp)
		fclose(dsentry->req_fp);

	if(dsentry->res_fp)
		fclose(dsentry->res_fp);

}

