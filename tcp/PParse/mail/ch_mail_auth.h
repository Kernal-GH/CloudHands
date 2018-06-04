#ifndef __CH_PARSER_MAIL_HANDLER_H__
#define __CH_PARSER_MAIL_HANDLER_H__
/*
 *
 *      Filename: ./ch_parser_mail_hanlder.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-12 15:40:52
 * Last Modified: 2016-07-12 15:40:52
 */

#include "ch_mpool.h"
#include "ch_string.h"

extern int 
ch_mail_auth_plain(ch_pool_t *mp, ch_str_t *user, ch_str_t *pass, 
					const unsigned char *buf, uint32_t len);

extern int ch_mail_auth_login_username(ch_pool_t *mp, ch_str_t *user, const unsigned char *buf, uint32_t len);

extern int ch_mail_auth_login_password(ch_pool_t *mp, ch_str_t *pass, const unsigned char  *buf, uint32_t len);

extern int ch_mail_auth_login_cram_md5(ch_pool_t *mp, ch_str_t *user, 
					ch_str_t *pass, const unsigned char *buf, uint32_t len);

#endif

