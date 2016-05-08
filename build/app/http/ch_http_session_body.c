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
#include "ch_constants.h"
#include "ch_http_session_body.h"

ch_http_session_body_t * ch_http_session_body_create(apr_pool_t *mp,const char *fname,size_t body_len){

    ch_http_session_body_t *body;
    int fd;

    if(fname == NULL) {
        ch_log(CH_LOG_ERR,"Must specify a valid file name for creating http session body!");
        return NULL;
    }

    fd = ch_file_open_for_write_common(fname);
    if(fd<0){
        ch_log(CH_LOG_ERR,"Open file [%s] failed!",fname);
        return NULL;
    }

    body = (ch_http_session_body_t*)apr_palloc(mp,sizeof(ch_http_session_body_t));
    body->file.fd = fd;
    body->file.offset = 0;
    body->file.sys_offset = 0;

    body->fname = fname;
    body->chunk = NULL;
    body->body_len = body_len;
    body->body_len_accepted = 0;

    return body;
}

void ch_http_session_body_destroy(ch_http_session_body_t *body){
   
    if(body){
        
        close(body->file.fd);
    }
}


int ch_http_session_body_append(ch_http_session_body_t *body,void *data,size_t data_len){

    ssize_t written;

    if(data == NULL || data_len == 0 ){
        // nothing to do
        return CH_OK;
    }

    written = ch_file_write(&body->file,data,data_len,body->file.offset);

    if((written == -1)||(data_len!=written)){
        /*error*/
        ch_log(CH_LOG_ERR,"write data[%d] into body's file[%s] failed!",data_len,body->fname);
        return CH_ERROR;
    }

    body->body_len_accepted+=data_len;

    return CH_OK;
}
