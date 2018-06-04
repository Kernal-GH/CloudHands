/*
 * =====================================================================================
 *
 *       Filename:  ch_session_format.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月18日 18时23分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_SESSION_FORMAT_H
#define CH_SESSION_FORMAT_H

typedef struct ch_session_format_t ch_session_format_t;
typedef struct ch_session_format_result_t ch_session_format_result_t;

#include "ch_file_store.h"
#include "ch_session_entry.h"

struct ch_session_format_t {

	ch_file_store_t *fstore;

    int (*format1)(ch_session_format_t *fmt,ch_session_entry_t *sentry,
		void *session,ch_session_format_result_t *result,void *priv_data);


	int (*format2)(ch_session_format_t *fmt,void *session,void *priv_data);
};

struct ch_session_format_result_t {

    void *data;
    size_t dlen;
};

static inline void ch_session_format_init(ch_session_format_t *fmt,ch_file_store_t *fstore,
    int (*format)(ch_session_format_t *fmt,ch_session_entry_t *sentry,
		void *session,ch_session_format_result_t *result,void *priv_data)){

	fmt->fstore = fstore;
	fmt->format1 = format;
}

extern int ch_session_format(ch_session_format_t *fmt,ch_session_entry_t *sentry,void *session,void *priv_data);

extern void ch_session_format_flush(ch_session_format_t *fmt);

extern void ch_session_format_close(ch_session_format_t *fmt);

#endif /*CH_SESSION_FORMAT_H*/
