/*
 *
 *      Filename: ch_http_session_entry.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-25 20:47:22
 * Last Modified: 2016-07-25 20:47:22
 */

#ifndef CH_HTTP_SESSION_ENTRY_H
#define CH_HTTP_SESSION_ENTRY_H

typedef struct ch_http_session_entry_t ch_http_session_entry_t;

#include "ch_http_session.h"
#include "ch_buf.h"
#include "ch_list.h"
#include "ch_session_entry.h"

struct ch_http_session_entry_t {

	struct ch_list_head reqs;

	apr_pool_t *mp;

	ch_buf_t *cur_req_buf;
	ch_buf_t *cur_res_buf;

	ch_http_session_t *cur_req;
	ch_http_session_t *cur_res;

};

static inline void ch_http_session_entry_data_store_reset(ch_http_session_entry_t *sentry,int is_req){

	ch_buf_t *b = is_req?sentry->cur_req_buf:sentry->cur_res_buf;
	
	if(b){
		b->pos = b->start;
	
		b->last = b->start;
	}
}

static inline int ch_http_session_entry_has_remain_data(ch_http_session_entry_t *sentry,int is_req){

	ch_buf_t *b = is_req?sentry->cur_req_buf:sentry->cur_res_buf;

	return b&&(b->pos>b->start);

}

static inline void ch_http_sentry_session_add(ch_http_session_entry_t *sentry,ch_http_session_t *session){

	list_add_tail(&session->node,&sentry->reqs);
	
	if(session == sentry->cur_req)
		sentry->cur_req = NULL;
}

static inline void ch_http_sentry_session_remove(ch_http_session_entry_t *sentry,ch_http_session_t *session){

	list_del(&session->node);

	if(session == sentry->cur_req)
		sentry->cur_req = NULL;
	if(session == sentry->cur_res)
		sentry->cur_res = NULL;

	ch_http_session_body_destroy(session->req_body);
	ch_http_session_body_destroy(session->res_body);
}


extern int ch_http_session_entry_data_store(ch_http_session_entry_t *sentry,void *data,size_t dlen,int is_req);


extern ch_http_session_t * ch_http_sentry_session_get(ch_http_session_entry_t *sentry,int is_req);

extern void ch_http_sentry_init(ch_http_session_entry_t *hsentry,ch_session_entry_t *sentry,
	ch_mmap_file_entry_t *fentry,apr_pool_t *mp);

#endif /* CH_HTTP_SESSION_ENTRY_H */
