/*
 *
 *      Filename: ./ch_telnet_parser_do.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-12-08 14:25:00
 * Last Modified: 2016-12-08 14:25:00
 */

#ifndef __CH_TELNET_PARSER_DO_H__
#define __CH_TELNET_PARSER_DO_H__

#include "ch_proto_pool.h"
#include "ch_session_entry.h"

int ch_telnet_request_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

int ch_telnet_response_parse(ch_app_proto_t *aproto, 
	ch_session_entry_t *sentry, void *data, 
	uint32_t dlen, void *priv_data);

#endif //__CH_TELNET_PARSER_DO_H__

