/*
 * =====================================================================================
 *
 *       Filename:  test_header.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月23日 17时15分24秒
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

#define CH_HTTP_PARSE_HEADER_DONE         1
#define CH_HTTP_PARSE_INVALID_HEADER      13

#define LF     (unsigned char) '\n'
#define CR     (unsigned char) '\r'
#define CRLF   "\r\n"

#define CH_OK 0

typedef struct {

    unsigned char *start;       /*start address waited to parse*/
    unsigned char *pos;         /*the position waiting to handle*/
    unsigned char *end;    /* end address waited to parse */

    unsigned state; /*the current parsed state*/
}ch_http_session_parse_t;


typedef struct {

    ch_http_session_parse_t parser;
    unsigned char                           *header_name_start;
    unsigned char                           *header_name_end;
    unsigned char                           *header_start;
    unsigned char                           *header_end;
    unsigned                          invalid_header:1;
} ch_http_header_t;


static int http_session_header_line_parser(ch_http_header_t *header)
{
    unsigned char      c, ch, *p;
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

    ch_http_session_parse_t *parser = &header->parser;

    /* the last '\0' is not needed because string is zero terminated */

    static unsigned char  lowcase[] =
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0-\0\0" "0123456789\0\0\0\0\0\0"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0_"
        "\0abcdefghijklmnopqrstuvwxyz\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    state = parser->state;

    for (p = parser->pos; p < parser->end; p++) {
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

                c = lowcase[ch];

                if (c) {
                    break;
                }

                if (ch == '\0') {
                    return CH_HTTP_PARSE_INVALID_HEADER;
                }

                header->invalid_header = 1;

                break;

            }
            break;

        /* header name */
        case sw_name:
            c = lowcase[ch];

            if (c) {
                break;
            }

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
                && strncmp(header->header_name_start, "HTTP", 4) == 0)
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

    return CH_HTTP_PARSE_INVALID_HEADER;

done:
    return CH_OK;

header_done:

    return CH_HTTP_PARSE_HEADER_DONE;
}
int main(int argc,char ** argv){

    if(argc<2){
        fprintf(stderr,"Usage:<request Line>\n");
        exit(-1);
    }

    unsigned char *line_str = argv[1];

    ch_http_header_t line;
    
    size_t len = strlen(line_str);

    unsigned char *start = (unsigned char*)malloc(len+2);
    memcpy(start,line_str,len);
    memcpy(start+len,"\r",1);
    memcpy(start+len+1,"\n",1);

    line.parser.state = 0;
    line.parser.start = start;
    line.parser.pos = start;
    line.parser.end = start+len+2;

    int rc =http_session_header_line_parser(&line);

    return 0;
}
