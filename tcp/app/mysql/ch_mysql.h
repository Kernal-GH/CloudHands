/*
 *
 *      Filename: ch_mysql.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-10-30 12:15:53
 * Last Modified: 2018-10-30 12:15:53
 */

#ifndef CH_MYSQL_H
#define CH_MYSQL_H

#include "ch_tcp_app_pool.h"

extern int ch_mysql_init(ch_tcp_app_pool_t *ta_pool,const char *cfname);

#endif /*CH_MYSQL_H*/
