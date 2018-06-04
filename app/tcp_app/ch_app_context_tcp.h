/*
 * =====================================================================================
 *
 *       Filename:  ch_app_context_tcp.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月1日 10时50分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_APP_CONTEXT_TCP_H
#define CH_APP_CONTEXT_TCP_H

typedef struct ch_app_context_tcp_t ch_app_context_tcp_t;

#include "ch_app_context.h"
#include "ch_mpool.h"

#define PORT_MAX_SIZE 218

struct ch_app_context_tcp_t {

    ch_app_context_t app_context;

    int http_is_on;
    int smtp_is_on;
    int pop3_is_on;
    int imap_is_on;
    int telnet_is_on;
    int ftp_is_on;
	int debug_is_on;

	uint16_t http_ports[PORT_MAX_SIZE];
	uint16_t smtp_ports[PORT_MAX_SIZE];
	uint16_t pop3_ports[PORT_MAX_SIZE];
	uint16_t imap_ports[PORT_MAX_SIZE];
	uint16_t telnet_ports[PORT_MAX_SIZE];
	uint16_t ftp_ports[PORT_MAX_SIZE];

	uint16_t debug_ports[PORT_MAX_SIZE];
};

extern ch_app_context_t * ch_app_context_tcp_create(ch_pool_t *mp,const char *cfname);

#endif /*CH_APP_CONTEXT_TCP_H*/
