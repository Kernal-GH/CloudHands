/*
 * =====================================================================================
 *
 *       Filename:  ch_http.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月24日 14时15分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_http.h"

static int http_recognize_by_port(ch_app_t *app,
        uint16_t src_port,uint16_t dst_port,void *priv_data){

    uint16_t min_port = src_port<=dst_port?src_port:dst_port;

    /*ignore https*/
    if(src_port == 443 || dst_port == 443)
        return 0;

    /*ignore ports that less than common port(80)*/
    if(min_port<app->common_port)
        return 0;

    
    /*maybe a http session,need to be recognize by its contents!*/
    return 1;
}

static int http_content_parse(ch_app_t *app,ch_assemble_session_t *ass,
        void *data,size_t dlen,void *priv_data){

}

static void http_content_flush(ch_app_t *app,ch_assemble_session_t *ass,void *priv_data){
    
}

static void http_content_close(ch_app_t *app,ch_assemble_session_t *ass,void *priv_data){

}

static ch_app_t http_app = {
    .common_port = 80,
    .priv_data = NULL,
    .app_recognize_by_port = http_recognize_by_port,
    .app_content_parse = http_content_parse,
    .app_content_flush = http_content_flush,
    .app_content_close = http_content_close,
};

int ch_http_init(ch_app_context_t *app_context){

    ch_app_context_register(app_context,&http_app);

    return 0;
}

