/*
 * =====================================================================================
 *
 *       Filename:  ch_http_session_parse.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月18日 15时23分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_HTTP_SESSION_PARSE
#define CH_HTTP_SESSION_PARSE

typedef struct {

    unsigned char *start;       /*start address waited to parse*/
    unsigned char *pos;         /*the position waiting to handle*/
    unsigned char *end;    /* end address waited to parse */

    unsigned state; /*the current parsed state*/
}ch_http_session_parse_t;

typedef struct {
    ch_http_session_parse_t parser;
    unsigned int           http_version;
    unsigned int           code;
    unsigned int           count;
    unsigned char          *start;
    unsigned char          *end;
    unsigned               http_minor:16;
    unsigned               http_major:16;
} ch_http_status_line_t;

typedef struct {

    ch_http_session_parse_t parser;
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

    unsigned                          http_minor:16;
    unsigned                          http_major:16;

}ch_http_request_line_t;

typedef struct {

    ch_http_session_parse_t parser;
    unsigned char                           *header_name_start;
    unsigned char                           *header_name_end;
    unsigned char                           *header_start;
    unsigned char                           *header_end;
    unsigned                          invalid_header:1;
} ch_http_header_t;

typedef struct {
    ch_http_session_parse_t parser;
    size_t size;
    size_t length;
}ch_http_body_chunk_t;


extern int ch_http_session_parse(ch_http_session_t *session,ch_http_session_buf *sbuf);

#endif /*CH_HTTP_SESSION_PARSE*/

