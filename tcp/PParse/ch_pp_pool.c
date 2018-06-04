/*
 *
 *      Filename: ch_pp_pool.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-17 14:55:00
 * Last Modified: 2018-05-18 18:24:04
 */

#include "ch_pp_pool.h"
#include "ch_log.h"
#include "ch_http_parser.h"
#include "ch_mail_parser.h"

#define IS_SESSION_CLOSE(tcp_record) ((tcp_record)->packet_type == PACKET_TYPE_FLUSH || (tcp_record)->packet_type ==PACKET_TYPE_CLOSE)

static void _session_entry_timeout_cb(ch_ptable_entry_t *entry,uint64_t tv ch_unused,void *priv_data ch_unused){

	ch_session_entry_t *sentry = (ch_session_entry_t*)entry;

	sentry->pp_parser->session_entry_clean(sentry);


}

static void _session_entry_clean(ch_session_entry_pool_t *sentry_pool,ch_session_entry_t *sentry){

	sentry->pp_parser->session_entry_clean(sentry);
	ch_session_entry_pool_entry_free(sentry_pool,sentry);

}

ch_pp_pool_t * ch_pp_pool_create(ch_pp_work_t *pwork){
 

	ch_pp_pool_t *pp_pool = NULL;
	ch_pp_context_t *pcontext = pwork->pcontext;

	pp_pool = (ch_pp_pool_t*)ch_palloc(pwork->mp,sizeof(*pp_pool));
	
	pp_pool->mp = pwork->mp;
	pp_pool->pwork = pwork;
	pp_pool->pp_parsers = ch_array_make(pwork->mp,16,sizeof(ch_pp_parser_t*));
	
	pp_pool->sstore = ch_session_store_create(pwork->mp,pcontext); 
	if(pp_pool->sstore == NULL){
	
		ch_log(CH_LOG_ERR,"Create session store failed!");
		return NULL;
	}


	pp_pool->sentry_pool = ch_session_entry_pool_create(pcontext,_session_entry_timeout_cb,(void*)pp_pool); 

	if(pp_pool->sentry_pool == NULL){
	
		ch_log(CH_LOG_ERR,"Create session entry pool failed!");
		return NULL;
	}

	/*init http parser*/
	if(ch_http_parser_init(pp_pool,pcontext->http_pp_cfile)){
	
		ch_log(CH_LOG_ERR,"Cannot register http proto parser!");
		return NULL;
	}
	
	/*init mail parser*/
	if(ch_mail_parser_init(pp_pool,pcontext->mail_pp_cfile)){
	
		ch_log(CH_LOG_ERR,"Cannot register mail proto parser!");
		return NULL;
	}


	return pp_pool;
}




void ch_pp_session_parse(ch_pp_pool_t *pp_pool,ch_tcp_record_t *tcp_record){

	int rc;
	void *data;
	size_t dlen;

	size_t c = 0;
	ch_session_entry_t *sentry = NULL;
	ch_session_entry_pool_t *sentry_pool = pp_pool->sentry_pool;
	ch_pp_parser_t *pp_parser;

	sentry = ch_session_entry_pool_entry_find(sentry_pool,tcp_record);
	if(sentry == NULL){
	
		/*Try to create a new session entry*/
		pp_parser = ch_pp_parser_find(pp_pool,tcp_record);
		if(pp_parser == NULL){
		
			ch_log(CH_LOG_DEBUG,"Cannot find Proto parser for the record:%d",(int)tcp_record->protocol_id);
			return;
		}

		sentry = ch_session_entry_pool_entry_create(sentry_pool,pp_parser,tcp_record);
		if(sentry == NULL){
		
			ch_log(CH_LOG_ERR,"Cannot create a new session entry for the record!");
			return;
		}

	}

	data = tcp_record->shm_rcd.data;
	dlen = tcp_record->shm_rcd.data_len;

	if(IS_SESSION_CLOSE(tcp_record)){

		_session_entry_clean(sentry_pool,sentry);
	}else if(data&&dlen>0) {
	
		pp_parser = sentry->pp_parser;

		if(tcp_record->session_direct == SESSION_DIRECT_REQ){
		
			rc = pp_parser->request_parse(sentry,data,dlen); 
		}else{
		
			rc = pp_parser->response_parse(sentry,data,dlen); 
		}

		if(rc == PARSE_BREAK||rc == PARSE_DONE){

			_session_entry_clean(sentry_pool,sentry);
		}
	}

	c = ch_session_entry_pool_timeout_free(sentry_pool);
	if(c){
	
		ch_session_entry_pool_tbl_dump(sentry_pool);
	}

}


