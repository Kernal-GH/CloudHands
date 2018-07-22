/*
 *
 *      Filename: ch_parser_mail_hanlder.c
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-12 15:37:58
 * Last Modified: 2018-05-18 11:37:56
 */

#include "ch_mail_auth.h"
#include "ch_log.h"

int 
ch_mail_auth_plain(ch_pool_t *mp, ch_str_t *user, ch_str_t *pass, 
					const unsigned char *buf, uint32_t len)
{
	unsigned char *ptr;
	unsigned char *end;
	ch_str_t plain;
	ch_str_t src;

	plain.data = (unsigned char *)ch_pcalloc(mp, len);
	if (plain.data == NULL) {
		return -1;
	}

	src.data = (unsigned char *)buf;
	src.len = len;

	if (ch_decode_base64(&plain, &src) != 0) {
		return -1;
	}

	ptr = plain.data;
	end = plain.data + plain.len;

	while (ptr < end && *ptr++);
	user->data = ptr;

	while (ptr < end && *ptr++);

	user->len = ptr - user->data;

	pass->data = ptr;
	pass->len = end - ptr;

	return 0;
}

int ch_mail_auth_login_username(ch_pool_t *mp, ch_str_t *user, const unsigned char *buf, uint32_t len)
{
	ch_str_t src;

	user->data = (unsigned char *)ch_pcalloc(mp, len);
	if (user->data == NULL)
		return -1;

	src.data = (unsigned char *)buf;
	src.len = len;

	if (ch_decode_base64(user, &src) != 0) {
		CH_ERR_LOG("%s: decode base64 error.\n", __func__);
		return -1;
	}

	return 0;
}

int ch_mail_auth_login_password(ch_pool_t *mp, ch_str_t *pass, const unsigned char *buf, uint32_t len)
{
	ch_str_t src;

	pass->data = (unsigned char *)ch_pcalloc(mp, len);
	if (pass->data == NULL)
		return -1;

	src.data = (unsigned char *)buf;
	src.len = len;

	if (ch_decode_base64(pass, &src) != 0) {
		CH_ERR_LOG("%s: decode base64 error.\n", __func__);
		return -1;
	}

	return 0;
}

int ch_mail_auth_login_cram_md5(ch_pool_t *mp, ch_str_t *user, 
					ch_str_t *pass, const unsigned char *buf, uint32_t len)
{
	unsigned char *ptr, *end;
	ch_str_t plain;
	ch_str_t src;

	plain.data = (unsigned char *)ch_pcalloc(mp, len);
	if (plain.data == NULL) {
		return -1;
	}

	src.data = (unsigned char *)buf;
	src.len = len;

	if (ch_decode_base64(&plain, &src) != 0) {
		CH_ERR_LOG("%s: decode base64 error.\n", __func__);
		return -1;
	}

	ptr = plain.data;
	end = plain.data + plain.len;

	user->data = ptr;
	while (ptr < end ) {
		if (*ptr++ == ' ') {
			user->len = ptr - user->data - 1;
			pass->len = end - ptr;
			pass->data = ptr;
			break;
		}
	}

	return 0;
}

