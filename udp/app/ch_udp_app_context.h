/*
 *
 *      Filename: ch_udp_app_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-16 11:38:51
 * Last Modified: 2018-07-16 11:38:51
 */

#ifndef CH_UDP_APP_CONTEXT_H
#define CH_UDP_APP_CONTEXT_H

typedef struct ch_udp_app_context_t ch_udp_app_context_t;

#include "ch_mpool.h"

struct ch_udp_app_context_t {


	int dns_is_on;
	int tftp_is_on;
	int smon_is_on;

	const char *dns_cfname;
	const char *tftp_cfname;

	const char *smon_cfname;
};

extern ch_udp_app_context_t * ch_udp_app_context_create(ch_pool_t *mp,const char *cfname);


#endif /*CH_UDP_APP_CONTEXT_H*/
