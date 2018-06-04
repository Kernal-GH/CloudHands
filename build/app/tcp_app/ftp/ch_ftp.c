/*
 *
 *      Filename: ./app/tcp_app/ftp/ch_ftp.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-30 10:28:11
 * Last Modified: 2017-01-10 18:22:17
 */


#include <stdio.h>
#include <stdlib.h>

#include "ch_ftp.h"
#include "ch_app_util.h"
#include "ch_mmap_file_format.h"
#include "ch_tcp_data_writer.h"
#include "ch_app_context_tcp.h"

static int 
ftp_recognize_by_port(ch_app_t *app, 
	uint16_t src_port, uint16_t dst_port, void *priv_data)
{
	ch_app_context_tcp_t *tcontext = (ch_app_context_tcp_t*)app->app_context;

    return ch_ports_equal(tcontext->ftp_ports,PORT_MAX_SIZE,src_port,dst_port);
}

static int 
ftp_recognize_by_content(ch_app_t *app,void *data,size_t dlen,void *priv_data)
{
    return 1;
}

static int 
ftp_content_process(ch_app_t *app,void *data,size_t dlen,void *priv_data)
{
    return ch_tcp_data_write(priv_data,data,dlen,PACKET_TYPE_DATA,PROTOCOL_FTP);
}

static void 
ftp_content_flush(ch_app_t *app,void *priv_data)
{
    ch_tcp_data_write(priv_data,NULL,0,PACKET_TYPE_FLUSH,PROTOCOL_FTP);
}

static void 
ftp_content_close(ch_app_t *app,void *priv_data)
{
    ch_tcp_data_write(priv_data,NULL,0,PACKET_TYPE_CLOSE,PROTOCOL_FTP);
}

static ch_app_t ftp_app = {
    .app_recognize_by_port = ftp_recognize_by_port,
    .app_recognize_by_content = ftp_recognize_by_content,
    .app_content_process = ftp_content_process,
    .app_content_flush = ftp_content_flush,
    .app_content_close = ftp_content_close,
};

int ch_ftp_init(ch_app_context_t *app_context)
{
    ch_app_context_register(app_context,&ftp_app);

    return 0;
}
