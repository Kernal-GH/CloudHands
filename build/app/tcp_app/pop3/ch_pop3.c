/************************************************************************
 *
 *      Filename: app/pop3/a.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-06-23 03:35:33
 * Last Modified: 2016-12-05 19:48:17
 *************************************************************************/
#include "ch_pop3.h"
#include "ch_app_util.h"
#include "ch_mmap_file_format.h"
#include "ch_tcp_data_writer.h"
#include "ch_app_context_tcp.h"

static int pop3_recognize_by_port(ch_app_t *app, uint16_t src_port, 
			uint16_t dst_port, void *priv_data)
{
	ch_app_context_tcp_t *tcontext = (ch_app_context_tcp_t*)app->app_context;

    return ch_ports_equal(tcontext->pop3_ports,PORT_MAX_SIZE,src_port,dst_port);
    
}

static int pop3_recognize_by_content(ch_app_t *app, void *data,size_t dlen, void *priv_data)
{
    
    return 1;
}


static int pop3_content_process(ch_app_t *app,void *data, size_t dlen, void *priv_data)
{
    return ch_tcp_data_write(priv_data,data,dlen,PACKET_TYPE_DATA,PROTOCOL_POP3);
}

static void pop3_content_flush(ch_app_t *app,void *priv_data)
{
    ch_tcp_data_write(priv_data,NULL,0,PACKET_TYPE_FLUSH,PROTOCOL_POP3);
}

static void pop3_content_close(ch_app_t *app,void *priv_data)
{
    ch_tcp_data_write(priv_data,NULL,0,PACKET_TYPE_CLOSE,PROTOCOL_POP3);
}

static ch_app_t pop3_app = {
    .app_recognize_by_port = pop3_recognize_by_port,
    .app_recognize_by_content = pop3_recognize_by_content,
    .app_content_process = pop3_content_process,
    .app_content_flush = pop3_content_flush,
    .app_content_close = pop3_content_close,
};

int ch_pop3_init(ch_app_context_t *app_context)
{
    ch_app_context_register(app_context, &pop3_app);
    return 0;
}

