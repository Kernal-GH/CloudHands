/*
 *
 *      Filename: ch_session_format_factory.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-31 20:08:38
 * Last Modified: 2016-07-31 20:08:38
 */

#ifndef CH_SESSION_FORMAT_FACTORY_H
#define CH_SESSION_FORMAT_FACTORY_H

#include "ch_session_format.h"
#include "ch_session_format_msgpack.h"
#include "ch_file_store.h"
#include "ch_proto_context.h"

extern ch_session_format_t * 
ch_session_format_create(ch_proto_context_t *pcontext,ch_file_store_t *fstore,
	int (*format2)(ch_session_format_t *fmt,void *session,void *priv_data));


#endif /* CH_SESSION_FORMAT_FACTORY_H */
