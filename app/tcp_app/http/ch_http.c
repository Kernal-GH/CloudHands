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
#include "ch_util.h"
#include "ch_tcp_data_writer.h"
#include "ch_app_context_tcp.h"

static int http_recognize_by_port(ch_app_t *app,
        uint16_t src_port,uint16_t dst_port,void *priv_data){

	ch_app_context_tcp_t *tcontext = (ch_app_context_tcp_t*)app->app_context;

    return ch_ports_equal(tcontext->http_ports,PORT_MAX_SIZE,src_port,dst_port);

}

static int http_recognize_by_content(ch_app_t *app,void *data,size_t dlen,void *priv_data){

    return 1;
}

static int http_content_process(ch_app_t *app,ch_shm_format_t *fmt,ch_shm_record_t *rcd){

    return ch_tcp_data_write(fmt,rcd,PROTOCOL_HTTP);
}

static void http_content_flush(ch_app_t *app,ch_shm_format_t *fmt,ch_shm_record_t *rcd){

	ch_tcp_data_write(fmt,rcd,PROTOCOL_HTTP);
}

static void http_content_close(ch_app_t *app,ch_shm_format_t *fmt,ch_shm_record_t *rcd){

	ch_tcp_data_write(fmt,rcd,PROTOCOL_HTTP);
}

static ch_app_t http_app = {
    .app_recognize_by_port = http_recognize_by_port,
    .app_recognize_by_content = http_recognize_by_content,
    .app_content_process = http_content_process,
    .app_content_flush = http_content_flush,
    .app_content_close = http_content_close,
};

int ch_http_init(ch_app_context_t *app_context){

    ch_app_context_register(app_context,&http_app);

    return 0;
}

