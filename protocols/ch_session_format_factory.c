/*
 *
 *      Filename: ch_session_format_factory.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-31 20:13:31
 * Last Modified: 2016-10-28 01:48:05
 */

#include "ch_log.h"
#include "ch_session_format_factory.h"

ch_session_format_t * ch_session_format_create(ch_proto_context_t *pcontext,ch_file_store_t *fstore,
	int (*format2)(ch_session_format_t *fmt,void *session,void *priv_data)){


	ch_session_format_t *fmt = NULL;

	switch(pcontext->fmt_type){
	
	case FMT_TYPE_MSGPACK:

		fmt = ch_session_format_msgpack_create(pcontext->mp,fstore);

		break;

	case FMT_TYPE_JSON:
	case FMT_TYPE_XML:
	case FMT_TYPE_BIN:
	case FMT_TYPE_TXT:
	default:
		ch_log(CH_LOG_ERR,"unimplement the format type!");
	}

	fmt->format2 = format2;

	return fmt;

}
