/*
 * =====================================================================================
 *
 *       Filename:  ch_http_session.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月18日 12时05分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_HTTP_SESSION_H
#define CH_HTTP_SESSION_H

typedef struct ch_http_session_headers_in_t  ch_http_session_headers_in_t;

typedef struct ch_http_session_headers_out_t ch_http_session_headers_out_t;

typedef struct ch_http_session_t ch_http_session_t;

/*include hear*/
#include <apr_pools.h>
#include "ch_net_connection_info.h"
#include "ch_http_session_body.h"

#define LF     (unsigned char) '\n'
#define CR     (unsigned char) '\r'
#define CRLF   "\r\n"

#define CH_HTTP_VERSION_9                 9
#define CH_HTTP_VERSION_10                1000
#define CH_HTTP_VERSION_11                1001
#define CH_HTTP_VERSION_20                2000

#define CH_HTTP_UNKNOWN                   0x0001
#define CH_HTTP_GET                       0x0002
#define CH_HTTP_HEAD                      0x0004
#define CH_HTTP_POST                      0x0008
#define CH_HTTP_PUT                       0x0010
#define CH_HTTP_DELETE                    0x0020
#define CH_HTTP_MKCOL                     0x0040
#define CH_HTTP_COPY                      0x0080
#define CH_HTTP_MOVE                      0x0100
#define CH_HTTP_OPTIONS                   0x0200
#define CH_HTTP_PROPFIND                  0x0400
#define CH_HTTP_PROPPATCH                 0x0800
#define CH_HTTP_LOCK                      0x1000
#define CH_HTTP_UNLOCK                    0x2000
#define CH_HTTP_PATCH                     0x4000
#define CH_HTTP_TRACE                     0x8000

#define CH_HTTP_CONTINUE                  100
#define CH_HTTP_SWITCHING_PROTOCOLS       101
#define CH_HTTP_PROCESSING                102

#define CH_HTTP_OK                        200
#define CH_HTTP_CREATED                   201
#define CH_HTTP_ACCEPTED                  202
#define CH_HTTP_NO_CONTENT                204
#define CH_HTTP_PARTIAL_CONTENT           206

#define CH_HTTP_SPECIAL_RESPONSE          300
#define CH_HTTP_MOVED_PERMANENTLY         301
#define CH_HTTP_MOVED_TEMPORARILY         302
#define CH_HTTP_SEE_OTHER                 303
#define CH_HTTP_NOT_MODIFIED              304
#define CH_HTTP_TEMPORARY_REDIRECT        307

#define CH_HTTP_BAD_REQUEST               400
#define CH_HTTP_UNAUTHORIZED              401
#define CH_HTTP_FORBIDDEN                 403
#define CH_HTTP_NOT_FOUND                 404
#define CH_HTTP_NOT_ALLOWED               405
#define CH_HTTP_REQUEST_TIME_OUT          408
#define CH_HTTP_CONFLICT                  409
#define CH_HTTP_LENGTH_REQUIRED           411
#define CH_HTTP_PRECONDITION_FAILED       412
#define CH_HTTP_REQUEST_ENTITY_TOO_LARGE  413
#define CH_HTTP_REQUEST_URI_TOO_LARGE     414
#define CH_HTTP_UNSUPPORTED_MEDIA_TYPE    415
#define CH_HTTP_RANGE_NOT_SATISFIABLE     416

#define CH_HTTP_PARSE_HEADER_DONE         1

#define CH_HTTP_CLIENT_ERROR              10
#define CH_HTTP_PARSE_INVALID_METHOD      10
#define CH_HTTP_PARSE_INVALID_REQUEST     11
#define CH_HTTP_PARSE_INVALID_09_METHOD   12

#define CH_HTTP_PARSE_INVALID_HEADER      13
#define CH_AGAIN                          14
#define CH_DONE                           15

#define CH_MAX_SIZE_T_VALUE 4294967295 

enum {
    HS_STATE_START = 0,
    HS_STATE_REQ_LINE,
    HS_STATE_REQ_HEADER,
    HS_STATE_REQ_BODY,
    HS_STATE_RES_LINE,
    HS_STATE_RES_HEADER,
    HS_STATE_RES_BODY,
    HS_STATE_END
}http_session_state;

struct ch_http_session_t {

    ch_net_connection_info_t        con_info;    /*the network connection information*/
    ch_http_session_headers_in_t    headers_in;  /*the http request headers*/
    ch_http_session_headers_out_t   headers_out; /*the http response headers*/
    
    apr_pool_t                      *mp;         /*the memory pool for this session*/
    
    unsigned long req_tms;                      /*the http request arriving time ms*/
    unsigned long res_tms;                      /*the http response arriving time ms*/
    
    unsigned int http_method;
    unsigned int req_http_version;
    const char *uri;                        

    unsigned int res_http_version; /*response http protocol*/
    unsigned int status;  /*response code*/
    const char *phrase;   /*response code phrase*/

    /* used to parse HTTP headers */
    unsigned int                      state;

    ch_http_session_body_t *req_body; 

    ch_http_session_body_t *res_body;
};

/*set request line parsed to http session(set method and uri ...) 
 * @session,the instance of http session set
 * @parsed,the instance of http session parser */
extern void ch_http_session_request_line_parsed_set(ch_http_session_t *session,ch_http_session_parse_t *parsed);


/*add session header(request/response) into session headers
 * @session,the instance of http session
 * @parsed,the instance of http session header parsed
 * */
extern void ch_http_session_header_parsed_add(ch_http_session_t *session,ch_http_session_parse_t *parsed);


/*set response status line parsed to http session
 * @session,the instance of http session
 * @ parsed,the instance of http session parsed
*/
extern void ch_http_session_status_line_parsed_set(ch_http_session_t *session,ch_http_session_parse_t *parsed);

#endif /*CH_HTTP_SESSION_H*/

