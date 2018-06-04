/*
 *
 *      Filename: ch_debug.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 19:53:14
 * Last Modified: 2016-12-05 19:56:30
 */


#include "ch_debug.h"
#include "ch_app_util.h"
#include "ch_mmap_file_format.h"
#include "ch_tcp_data_writer.h"
#include "ch_app_context_tcp.h"

static int debug_recognize_by_port(ch_app_t *app,
        uint16_t src_port,uint16_t dst_port,void *priv_data){

	ch_app_context_tcp_t *tcontext = (ch_app_context_tcp_t*)app->app_context;

    return ch_ports_equal(tcontext->debug_ports,PORT_MAX_SIZE,src_port,dst_port);

}

static int debug_recognize_by_content(ch_app_t *app,void *data,size_t dlen,void *priv_data){

    return 1;
}

static int debug_content_process(ch_app_t *app,void *data,size_t dlen,void *priv_data){

    return ch_tcp_data_write(priv_data,data,dlen,PACKET_TYPE_DATA,PROTOCOL_DEBUG);
}

static void debug_content_flush(ch_app_t *app,void *priv_data){

    ch_tcp_data_write(priv_data,NULL,0,PACKET_TYPE_FLUSH,PROTOCOL_DEBUG);
}

static void debug_content_close(ch_app_t *app,void *priv_data){

    ch_tcp_data_write(priv_data,NULL,0,PACKET_TYPE_CLOSE,PROTOCOL_DEBUG);
}

static ch_app_t debug_app = {
    .app_recognize_by_port = debug_recognize_by_port,
    .app_recognize_by_content = debug_recognize_by_content,
    .app_content_process = debug_content_process,
    .app_content_flush = debug_content_flush,
    .app_content_close = debug_content_close,
};

int ch_debug_init(ch_app_context_t *app_context){

    ch_app_context_register(app_context,&debug_app);

    return 0;
}
