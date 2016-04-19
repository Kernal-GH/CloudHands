/*
 * =====================================================================================
 *
 *       Filename:  test_request_line.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月23日 14时49分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define LF     (unsigned char) '\n'
#define CR     (unsigned char) '\r'
#define CRLF   "\r\n"

#define CH_OK 0

#define CH_HTTP_PARSE_INVALID_METHOD      10
#define CH_HTTP_PARSE_INVALID_REQUEST     11
#define CH_HTTP_PARSE_INVALID_09_METHOD   12

typedef struct {

    unsigned char *start;       /*start address waited to parse*/
    unsigned char *pos;         /*the position waiting to handle*/
    unsigned char *end;    /* end address waited to parse */

    unsigned state; /*the current parsed state*/
}ch_http_session_parse_t;

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

static uint32_t  usual[] = {
    0xffffdbfe, /* 1111 1111 1111 1111  1101 1011 1111 1110 */

                /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
    0x7fff37d6, /* 0111 1111 1111 1111  0011 0111 1101 0110 */

                /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
};

static int http_session_request_line_parser(ch_http_request_line_t *line)
{
    unsigned char  c, ch, *p, *m;
    enum {
        sw_start = 0,
        sw_method,
        sw_spaces_before_uri,
        sw_schema,
        sw_schema_slash,
        sw_schema_slash_slash,
        sw_host_start,
        sw_host,
        sw_host_end,
        sw_host_ip_literal,
        sw_port,
        sw_host_http_09,
        sw_after_slash_in_uri,
        sw_check_uri,
        sw_check_uri_http_09,
        sw_uri,
        sw_http_09,
        sw_http_H,
        sw_http_HT,
        sw_http_HTT,
        sw_http_HTTP,
        sw_first_major_digit,
        sw_major_digit,
        sw_first_minor_digit,
        sw_minor_digit,
        sw_spaces_after_digit,
        sw_almost_done
    } state;

    ch_http_session_parse_t *parser = &line->parser;
    state = parser->state;

    for (p = parser->pos; p < parser->end; p++) {
        ch = *p;

        switch (state) {

        /* HTTP methods: GET, HEAD, POST */
        case sw_start:
            line->request_start = p;

            if (ch == CR || ch == LF) {
                break;
            }

            if ((ch < 'A' || ch > 'Z') && ch != '_') {
                return CH_HTTP_PARSE_INVALID_METHOD;
            }

            state = sw_method;
            break;

        case sw_method:
            if (ch == ' ') {
                line->method_end = p;
                state = sw_spaces_before_uri;
                break;
            }

            if ((ch < 'A' || ch > 'Z') && ch != '_') {
                return CH_HTTP_PARSE_INVALID_METHOD;
            }

            break;

        /* space* before URI */
        case sw_spaces_before_uri:

            if (ch == '/') {
                line->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            }

            c = (unsigned char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                line->schema_start = p;
                state = sw_schema;
                break;
            }

            switch (ch) {
            case ' ':
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_schema:

            c = (unsigned char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                break;
            }

            switch (ch) {
            case ':':
                line->schema_end = p;
                state = sw_schema_slash;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_schema_slash:
            switch (ch) {
            case '/':
                state = sw_schema_slash_slash;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_schema_slash_slash:
            switch (ch) {
            case '/':
                state = sw_host_start;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_host_start:

            line->host_start = p;

            if (ch == '[') {
                state = sw_host_ip_literal;
                break;
            }

            state = sw_host;

            /* fall through */

        case sw_host:

            c = (unsigned char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                break;
            }

            if ((ch >= '0' && ch <= '9') || ch == '.' || ch == '-') {
                break;
            }

            /* fall through */

        case sw_host_end:

            line->host_end = p;

            switch (ch) {
            case ':':
                state = sw_port;
                break;
            case '/':
                line->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            case ' ':
                /*
                 * use single "/" from request line to preserve pointers,
                 * if request line will be copied to large client buffer
                 */
                line->uri_start = line->schema_end + 1;
                line->uri_end = line->schema_end + 2;
                state = sw_host_http_09;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_host_ip_literal:

            if (ch >= '0' && ch <= '9') {
                break;
            }

            c = (unsigned char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                break;
            }

            switch (ch) {
            case ':':
                break;
            case ']':
                state = sw_host_end;
                break;
            case '-':
            case '.':
            case '_':
            case '~':
                /* unreserved */
                break;
            case '!':
            case '$':
            case '&':
            case '\'':
            case '(':
            case ')':
            case '*':
            case '+':
            case ',':
            case ';':
            case '=':
                /* sub-delims */
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_port:
            if (ch >= '0' && ch <= '9') {
                break;
            }

            switch (ch) {
            case '/':
                line->port_end = p;
                line->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            case ' ':
                line->port_end = p;
                /*
                 * use single "/" from request line to preserve pointers,
                 * if request line will be copied to large client buffer
                 */
                line->uri_start = line->schema_end + 1;
                line->uri_end = line->schema_end + 2;
                state = sw_host_http_09;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* space+ after "http://host[:port] " */
        case sw_host_http_09:
            switch (ch) {
            case ' ':
                break;
            case CR:
                line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                line->http_minor = 9;
                goto done;
            case 'H':
                state = sw_http_H;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;


        /* check "/.", "//", "%", and "\" (Win32) in URI */
        case sw_after_slash_in_uri:

            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                state = sw_check_uri;
                break;
            }

            switch (ch) {
            case ' ':
                line->uri_end = p;
                state = sw_check_uri_http_09;
                break;
            case CR:
                line->uri_end = p;
                line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                line->uri_end = p;
                line->http_minor = 9;
                goto done;
            case '?':
                line->args_start = p + 1;
                state = sw_uri;
                break;
            case '\0':
                return CH_HTTP_PARSE_INVALID_REQUEST;
            default:
                state = sw_check_uri;
                break;
            }
            break;

        case sw_check_uri:

            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                break;
            }

            switch (ch) {
            case '/':
                line->uri_ext = NULL;
                state = sw_after_slash_in_uri;
                break;
            case '.':
                line->uri_ext = p + 1;
                break;
            case ' ':
                line->uri_end = p;
                state = sw_check_uri_http_09;
                break;
            case CR:
                line->uri_end = p;
                line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                line->uri_end = p;
                line->http_minor = 9;
                goto done;
            case '?':
                line->args_start = p + 1;
                state = sw_uri;
                break;
            case '\0':
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* space+ after URI */
        case sw_check_uri_http_09:
            switch (ch) {
            case ' ':
                break;
            case CR:
                line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                line->http_minor = 9;
                goto done;
            case 'H':
                state = sw_http_H;
                break;
            default:
                state = sw_check_uri;
                p--;
                break;
            }
            break;


        /* URI */
        case sw_uri:

            if (usual[ch >> 5] & (1 << (ch & 0x1f))) {
                break;
            }

            switch (ch) {
            case ' ':
                line->uri_end = p;
                state = sw_http_09;
                break;
            case CR:
                line->uri_end = p;
                line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                line->uri_end = p;
                line->http_minor = 9;
                goto done;
            case '\0':
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* space+ after URI */
        case sw_http_09:
            switch (ch) {
            case ' ':
                break;
            case CR:
                line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                line->http_minor = 9;
                goto done;
            case 'H':
                state = sw_http_H;
                break;
            default:
                state = sw_uri;
                p--;
                break;
            }
            break;

        case sw_http_H:
            switch (ch) {
            case 'T':
                state = sw_http_HT;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_http_HT:
            switch (ch) {
            case 'T':
                state = sw_http_HTT;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_http_HTT:
            switch (ch) {
            case 'P':
                state = sw_http_HTTP;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        case sw_http_HTTP:
            switch (ch) {
            case '/':
                state = sw_first_major_digit;
                break;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* first digit of major HTTP version */
        case sw_first_major_digit:
            if (ch < '1' || ch > '9') {
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }

            line->http_major = ch - '0';
            state = sw_major_digit;
            break;

        /* major HTTP version or dot */
        case sw_major_digit:
            if (ch == '.') {
                state = sw_first_minor_digit;
                break;
            }

            if (ch < '0' || ch > '9') {
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }

            line->http_major = line->http_major * 10 + ch - '0';
            break;

        /* first digit of minor HTTP version */
        case sw_first_minor_digit:
            if (ch < '0' || ch > '9') {
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }

            line->http_minor = ch - '0';
            state = sw_minor_digit;
            break;

        /* minor HTTP version or end of request line */
        case sw_minor_digit:
            if (ch == CR) {
                state = sw_almost_done;
                break;
            }

            if (ch == LF) {
                goto done;
            }

            if (ch == ' ') {
                state = sw_spaces_after_digit;
                break;
            }

            if (ch < '0' || ch > '9') {
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }

            line->http_minor = line->http_minor * 10 + ch - '0';
            break;

        case sw_spaces_after_digit:
            switch (ch) {
            case ' ':
                break;
            case CR:
                state = sw_almost_done;
                break;
            case LF:
                goto done;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        /* end of request line */
        case sw_almost_done:
            line->request_end = p - 1;
            switch (ch) {
            case LF:
                goto done;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
        }
    }

    return CH_HTTP_PARSE_INVALID_REQUEST;

done:

    if (line->request_end == NULL) {
        line->request_end = p;
    }

    parser->state = sw_start;

    return CH_OK;
}

int main(int argc,char ** argv){

    if(argc<2){
        fprintf(stderr,"Usage:<request Line>\n");
        exit(-1);
    }

    unsigned char *line_str = argv[1];

    ch_http_request_line_t line;
    
    size_t len = strlen(line_str);

    unsigned char *start = (unsigned char*)malloc(len+2);
    memcpy(start,line_str,len);
    memcpy(start+len,"\r",1);
    memcpy(start+len+1,"\n",1);

    line.parser.state = 0;
    line.parser.start = start;
    line.parser.pos = start;
    line.parser.end = start+len+2;

    int rc =http_session_request_line_parser(&line);

    return 0;
}
