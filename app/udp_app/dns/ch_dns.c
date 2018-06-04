/*
 *
 *      Filename: ch_dns.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-16 19:49:45
 * Last Modified: 2016-11-01 21:11:08
 */

#include "ch_dns.h"
#include "ch_app_util.h"
#include "ch_mmap_file_format.h"
#include "ch_udp_data_writer.h"

#define DNS_PORTS_N 1

static uint16_t dns_ports[DNS_PORTS_N] ={53};

static int dns_recognize_by_port(ch_app_t *app,
        uint16_t src_port,uint16_t dst_port,void *priv_data){

    //return ch_ports_equal(dns_ports,DNS_PORTS_N,src_port,dst_port);

	/* only handle dns answer */
	return src_port == 53;
}

static int dns_recognize_by_content(ch_app_t *app,void *data,size_t dlen,void *priv_data){

    return 1;
}

static int dns_content_process(ch_app_t *app,void *data,size_t dlen,void *priv_data){

    return ch_udp_data_write(priv_data,data,dlen,PACKET_TYPE_DATA,PROTOCOL_DNS);
}

static void dns_content_flush(ch_app_t *app,void *priv_data){

    ch_udp_data_write(priv_data,NULL,0,PACKET_TYPE_FLUSH,PROTOCOL_DNS);
}

static void dns_content_close(ch_app_t *app,void *priv_data){

    ch_udp_data_write(priv_data,NULL,0,PACKET_TYPE_CLOSE,PROTOCOL_DNS);
}

static ch_app_t dns_app = {
    .app_recognize_by_port = dns_recognize_by_port,
    .app_recognize_by_content = dns_recognize_by_content,
    .app_content_process = dns_content_process,
    .app_content_flush = dns_content_flush,
    .app_content_close = dns_content_close,
};

int ch_dns_init(ch_app_context_t *app_context){

    ch_app_context_register(app_context,&dns_app);

    return 0;
}
