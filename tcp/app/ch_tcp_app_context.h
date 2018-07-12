/*
 *
 *      Filename: ch_tcp_app_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 14:17:17
 * Last Modified: 2018-07-12 14:17:17
 */

#ifndef CH_TCP_APP_CONTEXT_H
#define CH_TCP_APP_CONTEXT_H

typedef struct ch_tcp_app_context_t ch_tcp_app_context_t;

#include "ch_mpool.h"

struct ch_tcp_app_context_t {

    int http_is_on;
    int smtp_is_on;
    int pop3_is_on;
    int imap_is_on;
    int telnet_is_on;
    int ftp_is_on;
	int smon_is_on;

	const char *http_cfname;
	const char *smtp_cfname;
	const char *pop3_cfname;
	const char *imap_cfname;
	const char *telnet_cfname;
	const char *ftp_cfname;
	const char *smon_cfname;

};

extern ch_tcp_app_context_t * ch_tcp_app_context_create(ch_pool_t *mp,const char *cfname);

#endif /*CH_TCP_APP_CONTEXT_H*/
