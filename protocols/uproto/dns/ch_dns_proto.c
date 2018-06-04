/*
 *
 *      Filename: ch_dns_proto.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-30 19:57:51
 * Last Modified: 2016-11-21 22:03:37
 */

#include "ch_log.h"
#include "ch_dns_proto.h"
#include "ch_dns_session.h"
#include "ch_session_format_msgpack.h"
#include "ch_session_format.h"
#include "ch_session_format_factory.h"
#include "ch_dns_parser.h"
#include "ch_dns_rdata_parser.h"
#include "ch_dns_parser_do.c"
#include "ch_dns_session_format_msgpack.c"

ch_app_proto_t * ch_dns_proto_create(ch_proto_context_t *pcontext){

	ch_dns_proto_t *dproto = NULL;
	ch_dns_context_t *dcontext = NULL;
	ch_app_proto_t *aproto = NULL;

	ch_file_store_t *fstore = NULL;
	apr_pool_t *mp = pcontext->mp;

	dproto = (ch_dns_proto_t*)apr_palloc(mp,sizeof(*dproto));

	dproto->dcontext = ch_dns_context_create(pcontext);

	if(dproto->dcontext == NULL){
	
		ch_log(CH_LOG_ERR,"create dns context failed!");
		return NULL;
	}

	aproto = &dproto->aproto;
	dcontext = dproto->dcontext;

	aproto->trans_proto_type = TPT_UDP;
	aproto->proto_id = PROTOCOL_DNS;  
	aproto->pcontext = pcontext;

	aproto->parse_request = _dns_parse_request;
	aproto->parse_response = _dns_parse_response;
	aproto->is_request = _dns_is_request;

	aproto->priv_data = (void*)dcontext;

	fstore = ch_file_store_create(pcontext->mp, dcontext->fstore_dir,
		dcontext->fstore_buf_size,dcontext->create_dir_type, pcontext->pid);

	if(fstore == NULL){
	
		ch_log(CH_LOG_ERR,"create file store failed for dns proto!");
		return NULL;
	}

	aproto->fmt = ch_session_format_create(pcontext,fstore,_dns_session_format);
	if(aproto->fmt == NULL){
	
		ch_log(CH_LOG_ERR,"create dns session format failed!");
		ch_file_store_close(fstore);
		return NULL;
	}


	dproto->cur_mp = NULL;
	dproto->sessions_parsed = 0;

	return (ch_app_proto_t*)dproto;

}

apr_pool_t * ch_dns_proto_mp_get(ch_dns_proto_t *dns_proto){

	apr_pool_t *mp;

	if(dns_proto->cur_mp == NULL ||dns_proto->sessions_parsed>dns_proto->dcontext->sessions_limit_mp){
	
		/*create a new memory pool*/
		if(APR_SUCCESS!=apr_pool_create(&mp,NULL)){
		
			/* return old memory pool */
			return dns_proto->cur_mp;
		}
		
		if(dns_proto->cur_mp){
		
			/* destroy  pre memory pool*/
			apr_pool_destroy(dns_proto->cur_mp);

		}
		
		dns_proto->cur_mp = mp;

	}
	
	return dns_proto->cur_mp;
	
}
