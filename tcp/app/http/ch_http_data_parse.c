/*
 * =====================================================================================
 *
 *       Filename:  ch_http_parser.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月18日 15时24分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "ch_constants.h"
#include "ch_http_data_parse.h"
#include "ch_http_session.h"

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

#define ch_str3_cmp(m, c0, c1, c2, c3)                                       \
    m[0] == c0 && m[1] == c1 && m[2] == c2

#define ch_str3Ocmp(m, c0, c1, c2, c3)                                       \
    m[0] == c0 && m[2] == c2 && m[3] == c3

#define ch_str4cmp(m, c0, c1, c2, c3)                                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3

#define ch_str5cmp(m, c0, c1, c2, c3, c4)                                    \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3 && m[4] == c4

#define ch_str6cmp(m, c0, c1, c2, c3, c4, c5)                                \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5

#define ch_str7_cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                       \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5 && m[6] == c6

#define ch_str8cmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                        \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7

#define ch_str9cmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)                    \
    m[0] == c0 && m[1] == c1 && m[2] == c2 && m[3] == c3                      \
        && m[4] == c4 && m[5] == c5 && m[6] == c6 && m[7] == c7 && m[8] == c8


int ch_http_request_line_parse(ch_http_request_line_t *request_line,ch_pp_data_line_t *line)
{
	unsigned char  c, ch, *p, *m;
	unsigned char *start = line->line;
	unsigned char *end = line->line+line->rawlen;

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

    state = 0;

    for (p = start; p < end; p++) {
        ch = *p;

        switch (state) {

        /* HTTP methods: GET, HEAD, POST */
        case sw_start:
            request_line->request_start = p;

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
                request_line->method_end = p - 1;
                m = request_line->request_start;

                switch (p - m) {

                case 3:
                    if (ch_str3_cmp(m, 'G', 'E', 'T', ' ')) {
                        request_line->method = CH_HTTP_GET;
                        break;
                    }

                    if (ch_str3_cmp(m, 'P', 'U', 'T', ' ')) {
                        request_line->method = CH_HTTP_PUT;
                        break;
                    }

                    break;

                case 4:
                    if (m[1] == 'O') {

                        if (ch_str3Ocmp(m, 'P', 'O', 'S', 'T')) {
                            request_line->method = CH_HTTP_POST;
                            break;
                        }

                        if (ch_str3Ocmp(m, 'C', 'O', 'P', 'Y')) {
                            request_line->method = CH_HTTP_COPY;
                            break;
                        }

                        if (ch_str3Ocmp(m, 'M', 'O', 'V', 'E')) {
                            request_line->method = CH_HTTP_MOVE;
                            break;
                        }

                        if (ch_str3Ocmp(m, 'L', 'O', 'C', 'K')) {
                            request_line->method = CH_HTTP_LOCK;
                            break;
                        }

                    } else {

                        if (ch_str4cmp(m, 'H', 'E', 'A', 'D')) {
                            request_line->method = CH_HTTP_HEAD;
                            break;
                        }
                    }

                    break;

                case 5:
                    if (ch_str5cmp(m, 'M', 'K', 'C', 'O', 'L')) {
                        request_line->method = CH_HTTP_MKCOL;
                        break;
                    }

                    if (ch_str5cmp(m, 'P', 'A', 'T', 'C', 'H')) {
                        request_line->method = CH_HTTP_PATCH;
                        break;
                    }

                    if (ch_str5cmp(m, 'T', 'R', 'A', 'C', 'E')) {
                        request_line->method = CH_HTTP_TRACE;
                        break;
                    }

                    break;

                case 6:
                    if (ch_str6cmp(m, 'D', 'E', 'L', 'E', 'T', 'E')) {
                        request_line->method = CH_HTTP_DELETE;
                        break;
                    }

                    if (ch_str6cmp(m, 'U', 'N', 'L', 'O', 'C', 'K')) {
                        request_line->method = CH_HTTP_UNLOCK;
                        break;
                    }

                    break;

                case 7:
                    if (ch_str7_cmp(m, 'O', 'P', 'T', 'I', 'O', 'N', 'S', ' '))
                    {
                        request_line->method = CH_HTTP_OPTIONS;
                    }

                    break;

                case 8:
                    if (ch_str8cmp(m, 'P', 'R', 'O', 'P', 'F', 'I', 'N', 'D'))
                    {
                        request_line->method = CH_HTTP_PROPFIND;
                    }

                    break;

                case 9:
                    if (ch_str9cmp(m,
                            'P', 'R', 'O', 'P', 'P', 'A', 'T', 'C', 'H'))
                    {
                        request_line->method = CH_HTTP_PROPPATCH;
                    }

                    break;
                }

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
                request_line->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            }

            c = (unsigned char) (ch | 0x20);
            if (c >= 'a' && c <= 'z') {
                request_line->schema_start = p;
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
                request_line->schema_end = p;
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

            request_line->host_start = p;

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

            request_line->host_end = p;

            switch (ch) {
            case ':':
                state = sw_port;
                break;
            case '/':
                request_line->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            case ' ':
                /*
                 * use single "/" from request request_line to preserve pointers,
                 * if request request_line will be copied to large client buffer
                 */
                request_line->uri_start = request_line->schema_end + 1;
                request_line->uri_end = request_line->schema_end + 2;
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
                request_line->port_end = p;
                request_line->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            case ' ':
                request_line->port_end = p;
                /*
                 * use single "/" from request request_line to preserve pointers,
                 * if request request_line will be copied to large client buffer
                 */
                request_line->uri_start = request_line->schema_end + 1;
                request_line->uri_end = request_line->schema_end + 2;
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
                request_line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                request_line->http_minor = 9;
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

            if (usual[ch >> 5] & (1U << (ch & 0x1f))) {
                state = sw_check_uri;
                break;
            }

            switch (ch) {
            case ' ':
                request_line->uri_end = p;
                state = sw_check_uri_http_09;
                break;
            case CR:
                request_line->uri_end = p;
                request_line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                request_line->uri_end = p;
                request_line->http_minor = 9;
                goto done;
            case '.':
                state = sw_uri;
                break;
            case '%':
                state = sw_uri;
                break;
            case '/':
                state = sw_uri;
                break;
            case '?':
                request_line->args_start = p + 1;
                state = sw_uri;
                break;
            case '#':
                state = sw_uri;
                break;
            case '+':
                break;
            case '\0':
                return CH_HTTP_PARSE_INVALID_REQUEST;
            default:
                state = sw_check_uri;
                break;
            }
            break;

        /* check "/", "%" and "\" (Win32) in URI */
        case sw_check_uri:

            if (usual[ch >> 5] & (1U << (ch & 0x1f))) {
                break;
            }

            switch (ch) {
            case '/':

                request_line->uri_ext = NULL;
                state = sw_after_slash_in_uri;
                break;
            case '.':
                request_line->uri_ext = p + 1;
                break;
            case ' ':
                request_line->uri_end = p;
                state = sw_check_uri_http_09;
                break;
            case CR:
                request_line->uri_end = p;
                request_line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                request_line->uri_end = p;
                request_line->http_minor = 9;
                goto done;

            case '%':
                state = sw_uri;
                break;
            case '?':
                request_line->args_start = p + 1;
                state = sw_uri;
                break;
            case '#':
                state = sw_uri;
                break;
            case '+':
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
                request_line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                request_line->http_minor = 9;
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

            if (usual[ch >> 5] & (1U << (ch & 0x1f))) {
                break;
            }

            switch (ch) {
            case ' ':
                request_line->uri_end = p;
                state = sw_http_09;
                break;
            case CR:
                request_line->uri_end = p;
                request_line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                request_line->uri_end = p;
                request_line->http_minor = 9;
                goto done;
            case '#':
                break;
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
                request_line->http_minor = 9;
                state = sw_almost_done;
                break;
            case LF:
                request_line->http_minor = 9;
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

            request_line->http_major = ch - '0';
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

            if (request_line->http_major > 99) {
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }

            request_line->http_major = request_line->http_major * 10 + ch - '0';
            break;

        /* first digit of minor HTTP version */
        case sw_first_minor_digit:
            if (ch < '0' || ch > '9') {
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }

            request_line->http_minor = ch - '0';
            state = sw_minor_digit;
            break;

        /* minor HTTP version or end of request request_line */
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

            if (request_line->http_minor > 99) {
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }

            request_line->http_minor = request_line->http_minor * 10 + ch - '0';
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

        /* end of request request_line */
        case sw_almost_done:
            request_line->request_end = p - 1;
            switch (ch) {
            case LF:
                goto done;
            default:
                return CH_HTTP_PARSE_INVALID_REQUEST;
            }
        }
    }

    return CH_AGAIN;

done:

    if (request_line->request_end == NULL) {
        request_line->request_end = p;
    }

    request_line->http_version = request_line->http_major * 1000 + request_line->http_minor;

    if (request_line->http_version == 9 && request_line->method != CH_HTTP_GET) {
        return CH_HTTP_PARSE_INVALID_09_METHOD;
    }

    return CH_OK;
}

int ch_http_header_line_parse(ch_http_header_t *header,ch_pp_data_line_t *line)
{

    unsigned char      ch, *p;
    
	enum {
        sw_start = 0,
        sw_name,
        sw_space_before_value,
        sw_value,
        sw_space_after_value,
        sw_ignore_line,
        sw_almost_done,
        sw_header_almost_done
    } state;

	unsigned char *start = line->line;
	unsigned char *end = line->line+line->rawlen;

    state = 0;

    for (p = start; p < end; p++) {
        ch = *p;

        switch (state) {

        /* first char */
        case sw_start:
            header->header_name_start = p;
            header->invalid_header = 0;

            switch (ch) {
            case CR:
                header->header_end = p;
                state = sw_header_almost_done;
                break;
            case LF:
                header->header_end = p;
                goto header_done;
            default:
                state = sw_name;

                if (ch == '\0') {
                    return CH_HTTP_PARSE_INVALID_HEADER;
                }

                header->invalid_header = 1;

                break;

            }
            break;

        /* header name */
        case sw_name:

            if (ch == ':') {
                header->header_name_end = p;
                state = sw_space_before_value;
                break;
            }

            if (ch == CR) {
                header->header_name_end = p;
                header->header_start = p;
                header->header_end = p;
                state = sw_almost_done;
                break;
            }

            if (ch == LF) {
                header->header_name_end = p;
                header->header_start = p;
                header->header_end = p;
                goto done;
            }

            /* IIS may send the duplicate "HTTP/1.1 ..." lines */
            if (ch == '/'
                && p - header->header_name_start == 4
                && strncmp((const char*)header->header_name_start, "HTTP", 4) == 0)
            {
                state = sw_ignore_line;
                break;
            }

            if (ch == '\0') {
                return CH_HTTP_PARSE_INVALID_HEADER;
            }

            header->invalid_header = 1;

            break;

        /* space* before header value */
        case sw_space_before_value:
            switch (ch) {
            case ' ':
                break;
            case CR:
                header->header_start = p;
                header->header_end = p;
                state = sw_almost_done;
                break;
            case LF:
                header->header_start = p;
                header->header_end = p;
                goto done;
            case '\0':
                return CH_HTTP_PARSE_INVALID_HEADER;
            default:
                header->header_start = p;
                state = sw_value;
                break;
            }
            break;

        /* header value */
        case sw_value:
            switch (ch) {
            case ' ':
                header->header_end = p;
                state = sw_space_after_value;
                break;
            case CR:
                header->header_end = p;
                state = sw_almost_done;
                break;
            case LF:
                header->header_end = p;
                goto done;
            case '\0':
                return CH_HTTP_PARSE_INVALID_HEADER;
            }
            break;

        /* space* before end of header line */
        case sw_space_after_value:
            switch (ch) {
            case ' ':
                break;
            case CR:
                state = sw_almost_done;
                break;
            case LF:
                goto done;
            case '\0':
                return CH_HTTP_PARSE_INVALID_HEADER;
            default:
                state = sw_value;
                break;
            }
            break;

        /* ignore header line */
        case sw_ignore_line:
            switch (ch) {
            case LF:
                state = sw_start;
                break;
            default:
                break;
            }
            break;

        /* end of header line */
        case sw_almost_done:
            switch (ch) {
            case LF:
                goto done;
            case CR:
                break;
            default:
                return CH_HTTP_PARSE_INVALID_HEADER;
            }
            break;

        /* end of header */
        case sw_header_almost_done:
            switch (ch) {
            case LF:
                goto header_done;
            default:
                return CH_HTTP_PARSE_INVALID_HEADER;
            }
        }
    }

    return CH_AGAIN;

done:


    return CH_OK;

header_done:


    return CH_HTTP_PARSE_HEADER_DONE;
}

int ch_http_status_line_parse(ch_http_status_line_t *status,ch_pp_data_line_t *line)
{

    unsigned char   ch;
    unsigned char  *p;
    enum {
        sw_start = 0,
        sw_H,
        sw_HT,
        sw_HTT,
        sw_HTTP,
        sw_first_major_digit,
        sw_major_digit,
        sw_first_minor_digit,
        sw_minor_digit,
        sw_status,
        sw_space_after_status,
        sw_status_text,
        sw_almost_done
    } state;

    state = 0;
	unsigned char *start = line->line;
	unsigned char *end = line->line+line->rawlen;

    for (p = start; p < end; p++) {
        ch = *p;

        switch (state) {

        /* "HTTP/" */
        case sw_start:
            switch (ch) {
            case 'H':
                state = sw_H;
                break;
            default:
                return CH_ERROR;
            }
            break;

        case sw_H:
            switch (ch) {
            case 'T':
                state = sw_HT;
                break;
            default:
                return CH_ERROR;
            }
            break;

        case sw_HT:
            switch (ch) {
            case 'T':
                state = sw_HTT;
                break;
            default:
                return CH_ERROR;
            }
            break;

        case sw_HTT:
            switch (ch) {
            case 'P':
                state = sw_HTTP;
                break;
            default:
                return CH_ERROR;
            }
            break;

        case sw_HTTP:
            switch (ch) {
            case '/':
                state = sw_first_major_digit;
                break;
            default:
                return CH_ERROR;
            }
            break;

        /* the first digit of major HTTP version */
        case sw_first_major_digit:
            if (ch < '1' || ch > '9') {
                return CH_ERROR;
            }

            status->http_major = ch - '0';
            state = sw_major_digit;
            break;

        /* the major HTTP version or dot */
        case sw_major_digit:
            if (ch == '.') {
                state = sw_first_minor_digit;
                break;
            }

            if (ch < '0' || ch > '9') {
                return CH_ERROR;
            }

            if (status->http_major > 99) {
                return CH_ERROR;
            }

            status->http_major = status->http_major * 10 + ch - '0';
            break;

        /* the first digit of minor HTTP version */
        case sw_first_minor_digit:
            if (ch < '0' || ch > '9') {
                return CH_ERROR;
            }

            status->http_minor = ch - '0';
            state = sw_minor_digit;
            break;

        /* the minor HTTP version or the end of the request line */
        case sw_minor_digit:
            if (ch == ' ') {
                state = sw_status;
                break;
            }

            if (ch < '0' || ch > '9') {
                return CH_ERROR;
            }

            if (status->http_minor > 99) {
                return CH_ERROR;
            }

            status->http_minor = status->http_minor * 10 + ch - '0';
            break;

        /* HTTP status code */
        case sw_status:
            if (ch == ' ') {
                break;
            }

            if (ch < '0' || ch > '9') {
                return CH_ERROR;
            }

            status->code = status->code * 10 + ch - '0';

            if (++status->count == 3) {
                state = sw_space_after_status;
                status->start = p - 2;
            }

            break;

        /* space or end of line */
        case sw_space_after_status:
            switch (ch) {
            case ' ':
                state = sw_status_text;
                break;
            case '.':                    /* IIS may send 403.1, 403.2, etc */
                state = sw_status_text;
                break;
            case CR:
                state = sw_almost_done;
                break;
            case LF:
                goto done;
            default:
                return CH_ERROR;
            }
            break;

        /* any text until end of line */
        case sw_status_text:
            switch (ch) {
            case CR:
                state = sw_almost_done;

                break;
            case LF:
                goto done;
            }
            break;

        /* end of status line */
        case sw_almost_done:
            status->end = p - 1;
            switch (ch) {
            case LF:
                goto done;
            default:
                return CH_ERROR;
            }
        }
    }

    return CH_AGAIN;

done:

    if (status->end == NULL) {
        status->end = p;
    }

    status->http_version = status->http_major * 1000 + status->http_minor;

    return CH_OK;
}

int ch_http_chunked_parse(ch_http_chunked_t *ctx){

    unsigned char     *pos, ch, c;
    int   rc;
    enum {
        sw_chunk_start = 0,
        sw_chunk_size,
        sw_chunk_extension,
        sw_chunk_extension_almost_done,
        sw_chunk_data,
        sw_after_data,
        sw_after_data_almost_done,
        sw_last_chunk_extension,
        sw_last_chunk_extension_almost_done,
        sw_trailer,
        sw_trailer_almost_done,
        sw_trailer_header,
        sw_trailer_header_almost_done
    } state;

    state = ctx->state;
	ch_pp_data_input_t *din = ctx->din;

    if (state == sw_chunk_data && ctx->size == 0) {
        state = sw_after_data;
    }

    rc = CH_AGAIN;

    for (pos = din->pos; pos < din->end; pos++) {

        ch = *pos;

        switch (state) {

        case sw_chunk_start:
            if (ch >= '0' && ch <= '9') {
                state = sw_chunk_size;
                ctx->size = ch - '0';
                break;
            }

            c = (unsigned char) (ch | 0x20);

            if (c >= 'a' && c <= 'f') {
                state = sw_chunk_size;
                ctx->size = c - 'a' + 10;
                break;
            }

            goto invalid;

        case sw_chunk_size:
            if (ctx->size > CH_MAX_OFF_T_VALUE / 16) {
                goto invalid;
            }

            if (ch >= '0' && ch <= '9') {
                ctx->size = ctx->size * 16 + (ch - '0');
                break;
            }

            c = (unsigned char) (ch | 0x20);

            if (c >= 'a' && c <= 'f') {
                ctx->size = ctx->size * 16 + (c - 'a' + 10);
                break;
            }

            if (ctx->size == 0) {

                switch (ch) {
                case CR:
                    state = sw_last_chunk_extension_almost_done;
                    break;
                case LF:
                    state = sw_trailer;
                    break;
                case ';':
                case ' ':
                case '\t':
                    state = sw_last_chunk_extension;
                    break;
                default:
                    goto invalid;
                }

                break;
            }

            switch (ch) {
            case CR:
                state = sw_chunk_extension_almost_done;
                break;
            case LF:
                state = sw_chunk_data;
                break;
            case ';':
            case ' ':
            case '\t':
                state = sw_chunk_extension;
                break;
            default:
                goto invalid;
            }

            break;

        case sw_chunk_extension:
            switch (ch) {
            case CR:
                state = sw_chunk_extension_almost_done;
                break;
            case LF:
                state = sw_chunk_data;
            }
            break;

        case sw_chunk_extension_almost_done:
            if (ch == LF) {
                state = sw_chunk_data;
                break;
            }
            goto invalid;

        case sw_chunk_data:
            rc = CH_OK;
            goto data;

        case sw_after_data:
            switch (ch) {
            case CR:
                state = sw_after_data_almost_done;
                break;
            case LF:
                state = sw_chunk_start;
            }
            break;

        case sw_after_data_almost_done:
            if (ch == LF) {
                state = sw_chunk_start;
                break;
            }
            goto invalid;

        case sw_last_chunk_extension:
            switch (ch) {
            case CR:
                state = sw_last_chunk_extension_almost_done;
                break;
            case LF:
                state = sw_trailer;
            }
            break;

        case sw_last_chunk_extension_almost_done:
            if (ch == LF) {
                state = sw_trailer;
                break;
            }
            goto invalid;

        case sw_trailer:
            switch (ch) {
            case CR:
                state = sw_trailer_almost_done;
                break;
            case LF:
                goto done;
            default:
                state = sw_trailer_header;
            }
            break;

        case sw_trailer_almost_done:
            if (ch == LF) {
                goto done;
            }
            goto invalid;

        case sw_trailer_header:
            switch (ch) {
            case CR:
                state = sw_trailer_header_almost_done;
                break;
            case LF:
                state = sw_trailer;
            }
            break;

        case sw_trailer_header_almost_done:
            if (ch == LF) {
                state = sw_trailer;
                break;
            }
            goto invalid;

        }
    }

data:

    ctx->state = state;
    din->pos = pos;

    if (ctx->size > CH_MAX_OFF_T_VALUE - 5) {
        goto invalid;
    }

    switch (state) {

    case sw_chunk_start:
        ctx->length = 3 /* "0" LF LF */;
        break;
    case sw_chunk_size:
        ctx->length = 1 /* LF */
                      + (ctx->size ? ctx->size + 4 /* LF "0" LF LF */
                                   : 1 /* LF */);
        break;
    case sw_chunk_extension:
    case sw_chunk_extension_almost_done:
        ctx->length = 1 /* LF */ + ctx->size + 4 /* LF "0" LF LF */;
        break;
    case sw_chunk_data:
        ctx->length = ctx->size + 4 /* LF "0" LF LF */;
        break;
    case sw_after_data:
    case sw_after_data_almost_done:
        ctx->length = 4 /* LF "0" LF LF */;
        break;
    case sw_last_chunk_extension:
    case sw_last_chunk_extension_almost_done:
        ctx->length = 2 /* LF LF */;
        break;
    case sw_trailer:
    case sw_trailer_almost_done:
        ctx->length = 1 /* LF */;
        break;
    case sw_trailer_header:
    case sw_trailer_header_almost_done:
        ctx->length = 2 /* LF LF */;
        break;

    }

    return rc;

done:

    ctx->state = 0;
    din->pos = pos + 1;

    return CH_DONE;

invalid:

    return CH_ERROR;

}
