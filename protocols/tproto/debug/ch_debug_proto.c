/*
 *
 *      Filename: ch_debug_proto.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 22:14:55
 * Last Modified: 2016-12-06 00:39:34
 */

#include <msgpack.h>
#include <apr_pools.h>
#include "ch_constants.h"
#include "ch_log.h"
#include "ch_debug_session_entry.h"
#include "ch_debug_proto.h"
#include "ch_session_format_msgpack.h"

#include "ch_debug_main.c"
#include "ch_debug_tbl_ops.c"
#include "ch_debug_session_format_msgpack.c"
#include "ch_table.h"
#include "ch_session_format_factory.h"

ch_app_proto_t * ch_debug_proto_create(apr_pool_t *mp,ch_proto_context_t *pcontext){

	ch_debug_proto_t *dpproto = NULL;
	ch_debug_context_t *dcontext = NULL;
	ch_app_proto_t *aproto = NULL;

	apr_pool_t *htbl_mp = NULL;

	ch_file_store_t *fstore = NULL;

	dpproto = (ch_debug_proto_t*)apr_palloc(mp,sizeof(*dpproto));

	dpproto->dcontext = ch_debug_context_create(pcontext);

	if(dpproto->dcontext == NULL){
	
		ch_log(CH_LOG_ERR,"create debug context failed!");
		return NULL;
	}

	aproto = &dpproto->aproto;
	dcontext = dpproto->dcontext;

	aproto->trans_proto_type = TPT_TCP;
	aproto->proto_id = PROTOCOL_DEBUG;  
	aproto->pcontext = pcontext;

	aproto->parse_request = debug_request_parse;
	aproto->parse_response = debug_response_parse;
	aproto->is_request = NULL;

	aproto->priv_data = (void*)dcontext;

	dpproto->req_fpath = ch_fpath_create(mp,dcontext->req_body_dir,dcontext->create_body_dir_type,0, pcontext->pid);
	dpproto->res_fpath = ch_fpath_create(mp,dcontext->res_body_dir,dcontext->create_body_dir_type,0, pcontext->pid);

	if(dpproto->req_fpath == NULL || dpproto->res_fpath == NULL){
	
		ch_log(CH_LOG_ERR,"create fpath failed for debug parser protocol!");
		return NULL;
	}

	fstore = ch_file_store_create(pcontext->mp, dcontext->fstore_dir,
		dcontext->fstore_buf_size,dcontext->create_dir_type, pcontext->pid);

	if(fstore == NULL){
	
		ch_log(CH_LOG_ERR,"create file store failed for debug proto!");
	    ch_fpath_fin(dpproto->req_fpath);
	    ch_fpath_fin(dpproto->res_fpath);
		return NULL;
	}

	aproto->fmt = ch_session_format_create(pcontext,fstore,debug_session_format);
	if(aproto->fmt == NULL){
	
		ch_log(CH_LOG_ERR,"create debug session format failed!");
		ch_file_store_close(fstore);
	    ch_fpath_fin(dpproto->req_fpath);
	    ch_fpath_fin(dpproto->res_fpath);
		return NULL;
	}

	apr_pool_create(&htbl_mp,NULL);
	if(htbl_mp == NULL){
	
		ch_log(CH_LOG_ERR,"create debug session table memory pool failed!");
		ch_file_store_close(fstore);
	    ch_fpath_fin(dpproto->req_fpath);
	    ch_fpath_fin(dpproto->res_fpath);
		return NULL;
	}

	aproto->htbl = ch_table_create(htbl_mp,0,
		dcontext->n_entries_limit,
		dcontext->tbl_size,
		dcontext->entry_timeout,
		dcontext->n_caches_limits,
		(void*)aproto,
		debug_session_entry_hash,
		debug_session_entry_equal,
		debug_session_entry_clean,
		debug_session_entry_create);

	if(aproto->htbl == NULL){
	
		ch_log(CH_LOG_ERR,"create debug session table failed!");
		ch_file_store_close(fstore);
	    ch_fpath_fin(dpproto->req_fpath);
	    ch_fpath_fin(dpproto->res_fpath);
		return NULL;

	}

	return (ch_app_proto_t*)dpproto;
}

