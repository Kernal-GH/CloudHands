/*
 *
 *      Filename: ch_http_ctypes.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-09-07 17:02:29
 * Last Modified: 2018-09-07 17:02:29
 */

#ifndef CH_HTTP_CTYPES_H
#define CH_HTTP_CTYPES_H

typedef struct ch_http_ctype_t ch_http_ctype_t;

struct ch_http_ctype_t {

	const char *ctype;
	const char *ext_name;
};


extern int ch_http_ctype_is_match(const char *ctype,const char *ext_name);

#endif /*CH_HTTP_CTYPES_H*/
