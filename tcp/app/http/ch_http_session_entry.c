/*
 *
 *      Filename: ch_http_session_entry.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-25 20:52:38
 * Last Modified: 2018-05-15 17:49:50
 */

#include "ch_log.h"
#include "ch_http_session_entry.h"

static inline ch_http_session_t * _res_session_get(ch_http_session_entry_t *hsentry){

	ch_http_session_t *session;

	if(hsentry->cur_res)
		return hsentry->cur_res;

	if(list_empty(&hsentry->reqs))
		return NULL;

	session = list_first_entry(&hsentry->reqs,ch_http_session_t,node);
	
	hsentry->cur_res = session;
	return session;
}

static inline ch_http_session_t * _req_session_get(ch_http_session_entry_t *hsentry){

	ch_http_session_t *session = NULL;

	if(hsentry->cur_req){
	
		session =  hsentry->cur_req;
	}else{
		/* need allocate a new http session instance */
		session = ch_http_session_create(hsentry->mp);
		hsentry->cur_req = session;
	}

	return session;
}

ch_http_session_t * ch_http_sentry_session_get(ch_http_session_entry_t *hsentry,int is_req){

	if(is_req)
		return _req_session_get(hsentry);

	return _res_session_get(hsentry);
}

void ch_http_sentry_init(ch_http_session_entry_t *hsentry,ch_buffer_t *g_buffer,ch_pool_t *mp){

	hsentry->mp = mp;
	INIT_LIST_HEAD(&hsentry->reqs);
	hsentry->cur_req = NULL;
	hsentry->cur_res = NULL;

	ch_pp_din_init(&hsentry->cur_req_din,g_buffer,mp);
	ch_pp_din_init(&hsentry->cur_res_din,g_buffer,mp);

}
