/*
 * =====================================================================================
 *
 *       Filename:  ch_session_format.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/18/2016 08:46:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (jianfeng sha), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_session_format.h"

static inline int _format_ok(ch_session_format_result_t *result,int rc){

	return (rc == 0)&&(result->data&&result->dlen>0);
}

int ch_session_format(ch_session_format_t *fmt,ch_session_entry_t *sentry,void *session,void *priv_data){

	ch_session_format_result_t res,*result = &res;
	result->data = NULL;
	result->dlen = 0;

    int rc = fmt->format1(fmt,sentry,session,result,priv_data);
	if(_format_ok(result,rc)){
	
		ch_file_store_write(fmt->fstore,result->data,result->dlen);
	}

	return 0;
}

void ch_session_format_flush(ch_session_format_t *fmt){

	ch_file_store_flush(fmt->fstore);
}

void ch_session_format_close(ch_session_format_t *fmt){

	ch_file_store_close(fmt->fstore);
}

