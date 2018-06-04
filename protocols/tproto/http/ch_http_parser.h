/*
 * =====================================================================================
 *
 *       Filename:  ch_http_parser.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月12日 16时54分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_HTTP_PARSER_H
#define CH_HTTP_PARSER_H

#include <stdint.h>
#include "ch_buf.h"

typedef struct {

	ch_buf_t *pb;
    unsigned state; /*the current parsed state*/
}ch_http_session_parse_data_t;

typedef struct {
    ch_http_session_parse_data_t pdata;
    unsigned int           http_version;
    unsigned int           code;
    unsigned int           count;
    unsigned char          *start;
    unsigned char          *end;
    unsigned               http_minor:16;
    unsigned               http_major:16;
} ch_http_status_line_t;

static inline void ch_http_status_line_init(ch_http_status_line_t *status,ch_buf_t *pb){

	status->pdata.pb = pb;
	status->pdata.state = 0;

	status->http_version = 0;
	status->code = 0;
	status->count = 0;
	status->start = NULL;
	status->end = NULL;
	status->http_minor = 0;
	status->http_major = 0;
}

typedef struct {

    ch_http_session_parse_data_t pdata;
    unsigned char                           *uri_start;
    unsigned char                           *uri_end;
    unsigned char                           *uri_ext;
    unsigned char                           *args_start;
    unsigned char                           *request_start;
    unsigned char                           *request_end;
    unsigned char                           *method_end;
    unsigned char                           *schema_start;
    unsigned char                           *schema_end;
    unsigned char                           *host_start;
    unsigned char                           *host_end;
    unsigned char                           *port_start;
    unsigned char                           *port_end;

	unsigned int							 method;
	unsigned int							 http_version;

    unsigned                          http_minor:16;
    unsigned                          http_major:16;
}ch_http_request_line_t;

static inline void ch_http_request_line_init(ch_http_request_line_t *line,ch_buf_t *pb){

	line->uri_start = NULL;
	line->uri_end = NULL;
	line->uri_ext = NULL;
	line->args_start = NULL;
	line->request_start = NULL;
	line->request_end = NULL;
	line->method_end = NULL;
	line->schema_start = NULL;
	line->schema_end = NULL;
	line->host_start = NULL;
	line->host_end = NULL;
	line->port_start = NULL;
	line->port_end = NULL;

	line->http_minor = 0;
	line->http_major = 0;

	line->pdata.pb = pb;
	line->pdata.state = 0;
}

typedef struct {

    ch_http_session_parse_data_t pdata;
    unsigned char                           *header_name_start;
    unsigned char                           *header_name_end;
    unsigned char                           *header_start;
    unsigned char                           *header_end;
    unsigned                          invalid_header:1;
} ch_http_header_t;

static inline void ch_http_header_init(ch_http_header_t *header,ch_buf_t *pb){

	header->pdata.pb = pb;
	header->pdata.state = 0;
	header->header_name_start = NULL;
	header->header_name_end = NULL;
	header->header_start = NULL;
	header->header_end = NULL;
	header->invalid_header = 0;
}

typedef struct {
    ch_http_session_parse_data_t pdata;
    size_t size;
    size_t length;
}ch_http_chunked_t;


extern int ch_http_request_line_parse(ch_http_request_line_t *line);

extern int ch_http_header_line_parse(ch_http_header_t *header);

extern int ch_http_status_line_parse(ch_http_status_line_t *status);

extern int ch_http_chunked_parse(ch_http_chunked_t *ctx);

#endif /*CH_HTTP_PARSER_H*/
