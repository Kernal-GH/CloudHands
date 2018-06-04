/*
 *
 *      Filename: ch_http_proto.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-07-19 23:44:58
 * Last Modified: 2016-11-05 20:51:51
 */
#include <msgpack.h>
#include <apr_pools.h>
#include "ch_constants.h"
#include "ch_http_session.h"
#include "ch_log.h"
#include "ch_http_session_entry.h"
#include "ch_http_proto.h"
#include "ch_session_format_msgpack.h"

#include "ch_http_parser_do.c"
#include "ch_http_session_tbl_ops.c"
#include "ch_http_session_format_msgpack.c"
#include "ch_table.h"
#include "ch_session_format_factory.h"

ch_app_proto_t * ch_http_proto_create(apr_pool_t *mp,ch_proto_context_t *pcontext){

	ch_http_proto_t *hpproto = NULL;
	ch_http_context_t *hcontext = NULL;
	ch_app_proto_t *aproto = NULL;

	apr_pool_t *htbl_mp = NULL;

	ch_file_store_t *fstore = NULL;

	hpproto = (ch_http_proto_t*)apr_palloc(mp,sizeof(*hpproto));

	hpproto->hcontext = ch_http_context_create(pcontext);

	if(hpproto->hcontext == NULL){
	
		ch_log(CH_LOG_ERR,"create http context failed!");
		return NULL;
	}

	aproto = &hpproto->aproto;
	hcontext = hpproto->hcontext;

	aproto->trans_proto_type = TPT_TCP;
	aproto->proto_id = PROTOCOL_HTTP;  
	aproto->pcontext = pcontext;

	aproto->parse_request = http_request_parse;
	aproto->parse_response = http_response_parse;
	aproto->is_request = NULL;

	aproto->priv_data = (void*)hcontext;

	hpproto->req_fpath = ch_fpath_create(mp,hcontext->req_body_dir,hcontext->create_body_dir_type,0, pcontext->pid);
	hpproto->res_fpath = ch_fpath_create(mp,hcontext->res_body_dir,hcontext->create_body_dir_type,0, pcontext->pid);

	if(hpproto->req_fpath == NULL || hpproto->res_fpath == NULL){
	
		ch_log(CH_LOG_ERR,"create fpath failed for http parser protocol!");
		return NULL;
	}

	hpproto->parser_buf = malloc(hcontext->parser_buf_size);
	if(hpproto->parser_buf == NULL){
	
		ch_log(CH_LOG_ERR,"ALLOCATE parser buffer failed for http proto!");
	    ch_fpath_fin(hpproto->req_fpath);
	    ch_fpath_fin(hpproto->res_fpath);

		return NULL;
	}

	hpproto->parser_buf_size = hcontext->parser_buf_size;

	fstore = ch_file_store_create(pcontext->mp, hcontext->fstore_dir,
		hcontext->fstore_buf_size,hcontext->create_dir_type, pcontext->pid);

	if(fstore == NULL){
	
		ch_log(CH_LOG_ERR,"create file store failed for http proto!");
		free(hpproto->parser_buf);
	    ch_fpath_fin(hpproto->req_fpath);
	    ch_fpath_fin(hpproto->res_fpath);
		return NULL;
	}

	aproto->fmt = ch_session_format_create(pcontext,fstore,http_session_format);
	if(aproto->fmt == NULL){
	
		ch_log(CH_LOG_ERR,"create http session format failed!");
		ch_file_store_close(fstore);
		free(hpproto->parser_buf);
	    ch_fpath_fin(hpproto->req_fpath);
	    ch_fpath_fin(hpproto->res_fpath);
		return NULL;
	}

	apr_pool_create(&htbl_mp,NULL);
	if(htbl_mp == NULL){
	
		ch_log(CH_LOG_ERR,"create http session table memory pool failed!");
		ch_file_store_close(fstore);
		free(hpproto->parser_buf);
	    ch_fpath_fin(hpproto->req_fpath);
	    ch_fpath_fin(hpproto->res_fpath);
		return NULL;
	}

	aproto->htbl = ch_table_create(htbl_mp,0,
		hcontext->n_entries_limit,
		hcontext->tbl_size,
		hcontext->entry_timeout,
		hcontext->n_caches_limits,
		(void*)aproto,
		http_session_entry_hash,
		http_session_entry_equal,
		http_session_entry_clean,
		http_session_entry_create);

	if(aproto->htbl == NULL){
	
		ch_log(CH_LOG_ERR,"create http session table failed!");
		ch_file_store_close(fstore);
		free(hpproto->parser_buf);
	    ch_fpath_fin(hpproto->req_fpath);
	    ch_fpath_fin(hpproto->res_fpath);
		return NULL;

	}

	return (ch_app_proto_t*)hpproto;
}

static inline int _remain_data_empty(ch_buf_t *b){

	return b == NULL || b->start == b->pos;
}


int ch_http_proto_buf_init(ch_http_proto_t *hpproto,ch_buf_t *pb,ch_buf_t *remain_data,
	void *data,size_t dlen){

	void *new_data,*p;

	size_t ttsize = 0;
	size_t remain_data_size = _remain_data_empty(remain_data)?0:(size_t)(remain_data->pos-remain_data->start);


	if(remain_data_size == 0){
	
		pb->start = data;
		pb->pos = pb->start;
		pb->last = pb->start;
		pb->end = pb->start+dlen;

		return 0;
	}

	ttsize = dlen+remain_data_size;
	if(ttsize>hpproto->parser_buf_size){
	
		/* realloc a buf */
		new_data = malloc(ttsize+4);
		if(new_data == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot allocate new memory to store parse data!");
			return -1;
		}
		/* ok,free pre parse buf */
		if(hpproto->parser_buf){
		
			free(hpproto->parser_buf);
		}

		hpproto->parser_buf = new_data;
		hpproto->parser_buf_size = ttsize;
	}

	/* copy data into parser buf */
	p = hpproto->parser_buf;
	
	
	memcpy(p,remain_data->start,remain_data_size);
	p+= remain_data_size;
	


	memcpy(p,data,dlen);
	
	/* init parse buf */
	pb->start = hpproto->parser_buf;
	pb->pos = pb->start;
	pb->last = pb->start;
	pb->end = pb->start+ttsize;

	/*reset remain_data */
	remain_data->pos = remain_data->start;
	remain_data->last = remain_data->start;
	return 0;
}

