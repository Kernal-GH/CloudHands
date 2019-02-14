/*
 * =====================================================================================
 *
 *       Filename:  ch_ssh.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/17/2018 02:21:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jacksha (shajf), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SSH_H
#define CH_SSH_H

#include "ch_tcp_app_pool.h"

extern int ch_ssh_init(ch_tcp_app_pool_t *ta_pool,const char *cfname);

#endif /* CH_SSH_H */
