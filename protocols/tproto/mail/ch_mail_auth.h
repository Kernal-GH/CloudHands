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

int 
ch_mail_auth_plain(apr_pool_t *pool, ch_str_t *user, ch_str_t *pass, 
					const char *buf, uint32_t len);

int ch_mail_auth_login_username(apr_pool_t *pool, ch_str_t *user, const uint8_t *buf, uint32_t len);

int ch_mail_auth_login_password(apr_pool_t *pool, ch_str_t *pass, const uint8_t *buf, uint32_t len);

int ch_mail_auth_login_cram_md5(apr_pool_t *pool, ch_str_t *user, 
					ch_str_t *pass, const char *buf, uint32_t len);

#endif

