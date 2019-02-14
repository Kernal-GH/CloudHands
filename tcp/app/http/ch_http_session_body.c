/*
 * =====================================================================================
 *
 *       Filename:  ch_http_session_body.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月24日 12时13分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_log.h"
#include "ch_http_session_body.h"
#include "ch_string.h"

ch_http_session_body_t * ch_http_session_body_create(ch_pool_t *mp,const char *fname,size_t body_len){

    ch_http_session_body_t *body;
    int fd;

    if(fname == NULL) {
        ch_log(CH_LOG_ERR,"Must specify a valid file name for creating http session body!");
        return NULL;
    }

    fd = ch_sfile_open_for_write_common(fname);
    if(fd<0){
        ch_log(CH_LOG_ERR,"Open file [%s] failed!",fname);
        return NULL;
    }

    body = (ch_http_session_body_t*)ch_palloc(mp,sizeof(ch_http_session_body_t));
    body->sfile.fd = fd;
    body->sfile.offset = 0;
    body->sfile.sys_offset = 0;
	body->saved = 0;

	body->mp = mp;
    body->fname = ch_pstrdup(mp, fname);
    body->chunk = NULL;
    body->body_len = body_len;
    body->body_len_accepted = 0;

    return body;
}

void ch_http_session_body_destroy(ch_http_session_body_t *body){
   
    if (body && body->saved == 0) {
        close(body->sfile.fd);
		body->saved = 1;
    }
}


int ch_http_body_append(ch_http_session_body_t *body,void *data,size_t data_len){

    ssize_t written;

    if(data == NULL || data_len == 0 ){
        // nothing to do
        return 0;
    }

    written = ch_sfile_write(&body->sfile,data,data_len,body->sfile.offset);

    if((written == -1)||(data_len!=(size_t)written)){
        /*error*/
        ch_log(CH_LOG_ERR,"write data[%d] into body's file[%s] failed!",data_len,body->fname);
        return -1;
    }

    body->body_len_accepted+=data_len;

    return 0;
}
