/*
 *
 *      Filename: ch_smon_session_entry.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-13 13:16:29
 * Last Modified: 2018-08-16 17:29:16
 */

#include "ch_smon_session_entry.h"
#include "ch_log.h"

void ch_smon_session_entry_init(ch_smon_session_entry_t *smon_entry,ch_pool_t *mp){

	ch_proto_session_entry_init(&smon_entry->psEntry);

	smon_entry->mp = mp;
	smon_entry->id = 0;
	smon_entry->req_content_fpath = NULL;
	smon_entry->res_content_fpath = NULL;
	smon_entry->req_content_fp = NULL;
	smon_entry->res_content_fp = NULL;
}

int  ch_smon_session_entry_fpath_init(ch_smon_session_entry_t *smon_entry,const char *fpath,int is_req){

	FILE *fp = fopen(fpath,"a+");
	if(fp == NULL){
	
		ch_log(CH_LOG_ERR,"cannot open file:%s to writing!",fpath);
		return -1;
	}

	if(is_req){

		smon_entry->req_content_fp = fp;
		smon_entry->req_content_fpath = ch_pstrdup(smon_entry->mp,fpath);

	}else{
		smon_entry->res_content_fp = fp;
		smon_entry->res_content_fpath = ch_pstrdup(smon_entry->mp,fpath);
	
	}

	return 0;

}

void ch_smon_session_entry_write(ch_smon_session_entry_t *smon_entry,void *data,size_t dlen,int is_req){

	FILE *fp = is_req?smon_entry->req_content_fp:smon_entry->res_content_fp;

	if(fp){
	
		fwrite(data,dlen,1,fp);
		fflush(fp);
	}

}

void ch_smon_session_entry_fin(ch_smon_session_entry_t *smon_entry){

	if(smon_entry->req_content_fp)
		fclose(smon_entry->req_content_fp);

	if(smon_entry->res_content_fp)
		fclose(smon_entry->res_content_fp);

}
