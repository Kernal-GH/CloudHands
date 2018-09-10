/*
 *
 *      Filename: ch_http_session_entry.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-15 17:30:06
 * Last Modified: 2018-05-15 17:30:06
 */

#ifndef CH_HTTP_SESSION_ENTRY_H
#define CH_HTTP_SESSION_ENTRY_H

typedef struct ch_http_session_entry_t ch_http_session_entry_t;

#include "ch_http_session.h"
#include "ch_list.h"

struct ch_http_session_entry_t {

	struct list_head reqs;
	
	ch_pp_data_input_t cur_req_din;
	ch_pp_data_input_t cur_res_din;

	ch_pool_t *mp;

	ch_http_session_t *cur_req;
	ch_http_session_t *cur_res;

};


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

static inline void ch_http_sentry_session_discard(ch_http_session_entry_t *sentry,ch_http_session_t *session){

	list_del(&session->node);

	if(session == sentry->cur_req)
		sentry->cur_req = NULL;
	if(session == sentry->cur_res)
		sentry->cur_res = NULL;

	if(session->req_body){
	
		ch_http_session_body_destroy(session->req_body);

		if(session->req_body->fname)
			unlink(session->req_body->fname);

	}

	if(session->res_body){
	
		ch_http_session_body_destroy(session->res_body);
		if(session->res_body->fname)
			unlink(session->res_body->fname);

	}

}

extern ch_http_session_t * ch_http_sentry_session_get(ch_http_session_entry_t *hsentry,int is_req);

extern void ch_http_sentry_init(ch_http_session_entry_t *hsentry,ch_buffer_t *g_buffer,ch_pool_t *mp);


#endif /*CH_HTTP_SESSION_ENTRY_H*/
