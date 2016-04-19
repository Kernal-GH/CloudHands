/*
 * =====================================================================================
 *
 *       Filename:  test_status_line.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月23日 17时33分51秒
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
#define CH_ERROR -1

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

static int http_session_status_line_parser(ch_http_status_line_t *status)
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

    ch_http_session_parse_t *parser = &status->parser;

    state = parser->state;

    for (p = parser->pos; p < parser->end; p++) {
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

    return CH_ERROR;

done:

    if (status->end == NULL) {
        status->end = p;
    }

    return CH_OK;
}

int main(int argc,char ** argv){

    if(argc<2){
        fprintf(stderr,"Usage:<request Line>\n");
        exit(-1);
    }

    unsigned char *line_str = argv[1];

    ch_http_status_line_t line;
    memset(&line,0,sizeof(ch_http_status_line_t));
    
    size_t len = strlen(line_str);

    unsigned char *start = (unsigned char*)malloc(len+2);
    memcpy(start,line_str,len);
    memcpy(start+len,"\r",1);
    memcpy(start+len+1,"\n",1);

    line.parser.state = 0;
    line.parser.start = start;
    line.parser.pos = start;
    line.parser.end = start+len+2;

    int rc =http_session_status_line_parser(&line);

    return 0;
}
