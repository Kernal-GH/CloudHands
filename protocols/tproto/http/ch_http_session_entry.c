/*
 *
 *      Filename: ch_http_session_entry.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-25 20:52:38
 * Last Modified: 2016-11-22 20:07:38
 */

#include "ch_log.h"
#include "ch_http_session_entry.h"

static ch_buf_t * _get_store_buf(ch_http_session_entry_t *hsentry,size_t dlen,int is_req){

	size_t bsize = 0;
	ch_buf_t *new_buf = NULL;
	ch_buf_t *old_buf = is_req?hsentry->cur_req_buf:hsentry->cur_res_buf;

	if(old_buf&&dlen<=((size_t)(old_buf->end-old_buf->start))){
	
		old_buf->pos = old_buf->start;
		old_buf->last = old_buf->start;
		return old_buf;
	}

	/* allocate a new buf */
	bsize = dlen*2;
	new_buf = (ch_buf_t*)apr_palloc(hsentry->mp,sizeof(*new_buf)+bsize);
	new_buf->start = (void*)(new_buf+1);
	new_buf->pos = new_buf->start;
	new_buf->last = new_buf->start;
	new_buf->end = new_buf->start+bsize;

	if(is_req)
		hsentry->cur_req_buf = new_buf;
	else
		hsentry->cur_res_buf = new_buf;

	return new_buf;
}


int ch_http_session_entry_data_store(ch_http_session_entry_t *hsentry,void *data,size_t dlen,int is_req){


	ch_buf_t *b = _get_store_buf(hsentry,dlen,is_req);

	if(b == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot alloacte a new buf to store data[len:%d]",(int)dlen);
		return -1;
	}

	memcpy(b->start,data,dlen);
	b->pos += dlen;

	return 0;
}

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

void ch_http_sentry_init(ch_http_session_entry_t *hsentry,
	ch_session_entry_t *sentry,ch_mmap_file_entry_t *fentry,apr_pool_t *mp){

	ch_session_entry_init(sentry,fentry);
	hsentry->mp = mp;
	CH_INIT_LIST_HEAD(&hsentry->reqs);
	hsentry->cur_req_buf = NULL;
	hsentry->cur_res_buf = NULL;
	hsentry->cur_req = NULL;
	hsentry->cur_res = NULL;

	sentry->priv_data = (void*)hsentry;
}
