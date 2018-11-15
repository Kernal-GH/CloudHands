/*
 *
 *      Filename: ch_smon_session.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-16 18:11:42
 * Last Modified: 2018-07-16 18:52:41
 */

#include "ch_smon_session.h"
#include "ch_log.h"

ch_smon_session_t * ch_smon_session_create(ch_pool_t *mp){

	ch_smon_session_t *smon_session = (ch_smon_session_t*)ch_palloc(mp,sizeof(*smon_session));

	smon_session->mp = mp;
	smon_session->req_content_fpath = NULL;
	smon_session->res_content_fpath = NULL;
	smon_session->req_content_fp = NULL;
	smon_session->res_content_fp = NULL;

	return smon_session;
}

void ch_smon_session_destroy(ch_smon_session_t *smon_session){

	if(smon_session->req_content_fp)
		fclose(smon_session->req_content_fp);
	
	if(smon_session->res_content_fp)
		fclose(smon_session->res_content_fp);

}

int ch_smon_session_content_fpath_init(ch_smon_session_t *smon_session,const char *fpath,int is_req){

	FILE *fp = fopen(fpath,"a+");
	if(fp == NULL){
	
		ch_log(CH_LOG_ERR,"cannot open file:%s to writing!",fpath);
		return -1;
	}

	if(is_req){

		smon_session->req_content_fp = fp;
		smon_session->req_content_fpath = ch_pstrdup(smon_session->mp,fpath);

	}else{
		smon_session->res_content_fp = fp;
		smon_session->res_content_fpath = ch_pstrdup(smon_session->mp,fpath);
	
	}

	return 0;

}

void ch_smon_session_content_write(ch_smon_session_t *smon_session,void *data,size_t dlen,int is_req){

	FILE *fp = is_req?smon_session->req_content_fp:smon_session->res_content_fp;

	if(fp){
	
		fwrite(data,dlen,1,fp);
		fflush(fp);
	}

}

