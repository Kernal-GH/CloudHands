/*
 *
 *      Filename: ch_http_session.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-15 13:58:19
 * Last Modified: 2018-05-15 13:58:19
 */

#ifndef CH_HTTP_SESSION_H
#define CH_HTTP_SESSION_H

typedef struct ch_http_session_t ch_http_session_t;
typedef struct ch_http_headers_in_t ch_http_headers_in_t;
typedef struct ch_http_headers_out_t ch_http_headers_out_t;

#include "ch_list.h"
#include "ch_http_session_body.h"
#include "ch_http_data_parse.h"
#include "ch_proto_session_entry.h"

#define CH_HTTP_MAX_URI_CHANGES           10


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

#define CH_HTTP_PARSE_HEADER_DONE         1

#define CH_HTTP_PARSE_INVALID_METHOD      10
#define CH_HTTP_PARSE_INVALID_REQUEST     11
#define CH_HTTP_PARSE_INVALID_09_METHOD   12

#define CH_HTTP_PARSE_INVALID_HEADER      13


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
#define CH_HTTP_MISDIRECTED_REQUEST       421

/* 498 is the canceled code for the requests with invalid host name */

/*
 * HTTP does not define the code for the case when a client closed
 * the connection while we are processing its request so we introduce
 * own code to log such situation when a client has closed the connection
 * before we even try to send the HTTP header to it
 */
#define CH_HTTP_CLIENT_CLOSED_REQUEST     499


#define CH_HTTP_INTERNAL_SERVER_ERROR     500
#define CH_HTTP_NOT_IMPLEMENTED           501
#define CH_HTTP_BAD_GATEWAY               502
#define CH_HTTP_SERVICE_UNAVAILABLE       503
#define CH_HTTP_GATEWAY_TIME_OUT          504
#define CH_HTTP_INSUFFICIENT_STORAGE      507


#define CH_HTTP_CONNECTION_CLOSE          1
#define CH_HTTP_CONNECTION_KEEP_ALIVE     2

typedef struct {
    struct list_head node;
    unsigned char *key;
    unsigned char *val;
}ch_table_elt_t;

struct ch_http_headers_in_t {
    struct list_head              headers;

    ch_table_elt_t                  *host;
    ch_table_elt_t                  *connection;
    ch_table_elt_t                  *user_agent;
    ch_table_elt_t                  *content_length;
    ch_table_elt_t                  *content_type;

    ch_table_elt_t                  *transfer_encoding;

	size_t							 content_length_n;

    unsigned                          connection_type:2;
    unsigned                          chunked:1;
};


struct ch_http_headers_out_t {
    struct list_head                        headers;
    ch_table_elt_t                  *server;
    ch_table_elt_t                  *connection;
    ch_table_elt_t                  *content_length;
    ch_table_elt_t                  *content_encoding;
    ch_table_elt_t                  *transfer_encoding;
    ch_table_elt_t                  *content_type;
    
	size_t                          content_length_n;

    unsigned                          connection_type:2;
	unsigned						  chunked:1;

};

/*Parse phase */
enum {
	PARSE_PHASE_REQ_LINE = 0,
	PARSE_PHASE_REQ_HEADER,
	PARSE_PHASE_REQ_BODY, 
	PARSE_PHASE_STATUS_LINE,
	PARSE_PHASE_RES_HEADER,
	PARSE_PHASE_RES_BODY,
	PARSE_PHASE_DONE
};

struct ch_http_session_t {

	ch_proto_session_entry_t psEntry;

	struct list_head				 node;
    ch_http_headers_in_t             headers_in;
    ch_http_headers_out_t            headers_out;

    ch_pool_t *mp;

	ch_http_session_body_t			*req_body;
	ch_http_session_body_t			*res_body;

	unsigned int						parse_phase;
    unsigned int                        http_version;
    unsigned char						*method;
	unsigned char                       *uri;
	unsigned char						*host;
	unsigned int						in_headers_n;
	unsigned int						out_headers_n;

	unsigned int						status_code;

};

static inline int is_http_session_keepalive(ch_http_session_t *s){

	if(s->http_version <= CH_HTTP_VERSION_10)
		return 0;

	return (s->headers_in.connection_type == CH_HTTP_CONNECTION_KEEP_ALIVE)&&\
										   (s->headers_out.connection_type == CH_HTTP_CONNECTION_KEEP_ALIVE);
}

static inline const char * ch_http_version_to_str(unsigned int http_version){

	const char *v = "unknown";

	switch(http_version){
	
	case CH_HTTP_VERSION_9:
		v = "HTTP/0.9";
		break;
	case CH_HTTP_VERSION_10:
		v = "HTTP/1.0";
		break;
	case CH_HTTP_VERSION_11:
		v = "HTTP/1.1";
		break;
	case CH_HTTP_VERSION_20:
		v = "HTTP/2.0";
		break;
	
	default:
		break;
	}

	return v;
}

#define HTTP_VERSION_MAKE(major,minor) (major*1000+minor)

#define HTTP_STRNDUP(mp,result,start,end,incr) do {		\
	if(start == NULL || end == NULL||start==end)		\
		result = (unsigned char*)"";					\
	else												\
	result = (unsigned char*)ch_pstrndup(mp,(const char*)start,(size_t)((end-start)+incr)); \
}while(0) 

#define STRTOSIZE(str,result) do{							\
	char *end;												\
	if(str == NULL)											\
		result = 0;											\
	else													\
		result = (size_t)strtoul((const char*)str,&end,10);	\
}while(0)

#define ch_http_body_is_chunked(session,is_req) (is_req?(session)->headers_in.chunked:(session)->headers_out.chunked)

#define ch_is_http_body_complete(session,is_req) \
	(is_req?(session->req_body->body_len_accepted>=session->headers_in.content_length_n):\
	 (session->res_body->body_len_accepted>=session->headers_out.content_length_n))

extern ch_http_session_t * ch_http_session_create(ch_pool_t *mp);

extern void ch_http_session_request_line_set(ch_http_session_t *session,ch_http_request_line_t *req_line);

extern void ch_http_session_header_add(ch_http_session_t *session,ch_http_header_t *header,int is_req);

extern void ch_http_session_status_line_set(ch_http_session_t *session,ch_http_status_line_t *status);

extern void ch_http_session_body_append(ch_http_session_t *session,unsigned char *data,size_t dlen,int is_req);

#endif /*CH_HTTP_SESSION_H*/
