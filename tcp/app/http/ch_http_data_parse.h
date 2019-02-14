/*
 * =====================================================================================
 *
 *       Filename:  ch_http_data_parse.h
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

#ifndef CH_HTTP_DATA_PARSE_H
#define CH_HTTP_DATA_PARSE_H

#include "ch_pp_data_input.h"

typedef struct {
    unsigned int           http_version;
    unsigned int           code;
    unsigned int           count;
    unsigned char          *start;
    unsigned char          *end;
    unsigned               http_minor:16;
    unsigned               http_major:16;
} ch_http_status_line_t;

static inline void ch_http_status_line_init(ch_http_status_line_t *status){

	status->http_version = 0;
	status->code = 0;
	status->count = 0;
	status->start = NULL;
	status->end = NULL;
	status->http_minor = 0;
	status->http_major = 0;
}

typedef struct {

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

static inline void ch_http_request_line_init(ch_http_request_line_t *line){

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

}

typedef struct {

    unsigned char                           *header_name_start;
    unsigned char                           *header_name_end;
    unsigned char                           *header_start;
    unsigned char                           *header_end;
    unsigned                          invalid_header:1;
} ch_http_header_t;

static inline void ch_http_header_init(ch_http_header_t *header){

	header->header_name_start = NULL;
	header->header_name_end = NULL;
	header->header_start = NULL;
	header->header_end = NULL;
	header->invalid_header = 0;
}

typedef struct {
    size_t size;
    size_t length;
	int state;
	ch_pp_data_input_t *din;
}ch_http_chunked_t;


extern int ch_http_request_line_parse(ch_http_request_line_t *request_line,ch_pp_data_line_t *line);

extern int ch_http_header_line_parse(ch_http_header_t *header,ch_pp_data_line_t *line);

extern int ch_http_status_line_parse(ch_http_status_line_t *status,ch_pp_data_line_t *line);

extern int ch_http_chunked_parse(ch_http_chunked_t *ctx);

#endif /*CH_HTTP_DATA_PARSE_H*/
