/*
 *
 *      Filename: ch_smtp.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-12 16:17:58
 * Last Modified: 2018-07-12 16:17:58
 */

#ifndef CH_SMTP_H
#define CH_SMTP_H

#include "ch_tcp_app_pool.h"

extern int ch_smtp_init(ch_tcp_app_pool_t *ta_pool,const char *cfname);

#endif /*CH_SMTP_H*/
