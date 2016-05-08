/*
 * =====================================================================================
 *
 *       Filename:  ch_http_session.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月18日 12时06分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "ch_http_session.h"

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

static void http_session_request_method_set(ch_http_session_t *session,unsigned char *m,unsigned char *e){
    
    switch (e - m) {

        case 3:
            if (ch_str3_cmp(m, 'G', 'E', 'T', ' ')) {
                session->http_method = CH_HTTP_GET;
                break;
            }

            if (ch_str3_cmp(m, 'P', 'U', 'T', ' ')) {
                session->http_method = CH_HTTP_PUT;
                break;
            }

            break;

        case 4:
            if (m[1] == 'O') {

                if (ch_str3Ocmp(m, 'P', 'O', 'S', 'T')) {
                    session->http_method = CH_HTTP_POST;
                    break;
                }

                if (ch_str3Ocmp(m, 'C', 'O', 'P', 'Y')) {
                    session->http_method = CH_HTTP_COPY;
                    break;
                }

                if (ch_str3Ocmp(m, 'M', 'O', 'V', 'E')) {
                    session->http_method = CH_HTTP_MOVE;
                    break;
                }

                if (ch_str3Ocmp(m, 'L', 'O', 'C', 'K')) {
                    session->http_method = CH_HTTP_LOCK;
                    break;
                }

            } else {

                if (ch_str4cmp(m, 'H', 'E', 'A', 'D')) {
                    session->http_method = CH_HTTP_HEAD;
                    break;
                }
            }

            break;

        case 5:
            if (ch_str5cmp(m, 'M', 'K', 'C', 'O', 'L')) {
                session->http_method = CH_HTTP_MKCOL;
                break;
            }

            if (ch_str5cmp(m, 'P', 'A', 'T', 'C', 'H')) {
                session->http_method = CH_HTTP_PATCH;
                break;
            }

            if (ch_str5cmp(m, 'T', 'R', 'A', 'C', 'E')) {
                session->http_method = CH_HTTP_TRACE;
                break;
            }

            break;

        case 6:
            if (ch_str6cmp(m, 'D', 'E', 'L', 'E', 'T', 'E')) {
                session->http_method = CH_HTTP_DELETE;
                break;
            }

            if (ch_str6cmp(m, 'U', 'N', 'L', 'O', 'C', 'K')) {
                session->http_method = CH_HTTP_UNLOCK;
                break;
            }

            break;

        case 7:
            if (ch_str7_cmp(m, 'O', 'P', 'T', 'I', 'O', 'N', 'S', ' '))
            {
                session->http_method = CH_HTTP_OPTIONS;
            }

            break;

        case 8:
            if (ch_str8cmp(m, 'P', 'R', 'O', 'P', 'F', 'I', 'N', 'D'))
            {
                session->http_method = CH_HTTP_PROPFIND;
            }

            break;

        case 9:
            if (ch_str9cmp(m,
                    'P', 'R', 'O', 'P', 'P', 'A', 'T', 'C', 'H'))
            {
                session->http_method = CH_HTTP_PROPPATCH;
            }

            break;
        default:
            session->http_method = CH_HTTP_UNKNOWN;
    }
}

void ch_http_session_request_line_parsed_set(ch_http_session_t *session,ch_http_session_parse_t *parsed){

    /*set http method*/

    session->http_version = parse->http_major * 1000 + parse->http_minor;

}

