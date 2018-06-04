/*
 *
 *      Filename: ch_proto_pool.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 20:32:20
 * Last Modified: 2016-11-02 03:29:56
 */

#include "ch_log.h"
#include "ch_proto_pool.h"
#include "ch_proto_tcp.h"
#include "ch_proto_udp.h"

ch_proto_pool_t* ch_proto_pool_create(ch_proto_context_t *pcontext){

	ch_proto_pool_t *ppool = (ch_proto_pool_t*)apr_palloc(pcontext->mp,sizeof(*ppool));

	ppool->pcontext = pcontext;

	ppool->trans_proto = NULL;

	switch(pcontext->trans_proto_type){
	
	case TPT_TCP:
		if(ch_proto_tcp_init(ppool)){
		
			ch_log(CH_LOG_ERR,"register tcp proto failed!");
			return NULL;
		}
		break;

	case TPT_UDP:
		if(ch_proto_udp_init(ppool)){
		
			ch_log(CH_LOG_ERR,"register udp proto failed!");
			return NULL;
		}

		break;

	default:
		ch_log(CH_LOG_ERR,"unknown proto!");
		return NULL;
	}

	return ppool;
}

static inline int _is_can_register(ch_proto_pool_t *ppool,ch_app_proto_t *aproto){

	if(aproto->trans_proto_type>=TPT_MAX||aproto->trans_proto_type!=ppool->pcontext->trans_proto_type){
	
		ch_log(CH_LOG_ERR,"Invalid transport protocol type:%d,must in range[0,%d]!",aproto->trans_proto_type,
			TPT_MAX);
		return 0;
	}

	if(aproto->proto_id>= PROTOCOL_MAX){
	
		ch_log(CH_LOG_ERR,"Invalid app protocol id:%d,must in range[0,%d]!",aproto->proto_id,
			PROTOCOL_MAX);
		
		return 0;
	}

	return 1;
}

static inline ch_app_proto_t * _app_proto_get(ch_proto_pool_t *ppool,ch_mmap_file_entry_t *fe){

	if(fe->protocol_id>=PROTOCOL_MAX){
	
		ch_log(CH_LOG_ERR,"Unknown protocol id:%d,discard this packet!",(int)fe->protocol_id);
		return NULL;
	}

	return ppool->trans_proto->app_protos[fe->protocol_id];
}

void ch_app_proto_register(ch_proto_pool_t *ppool,ch_app_proto_t *aproto){

	if(_is_can_register(ppool,aproto))
		ppool->trans_proto->app_protos[aproto->proto_id] = aproto;
}

void ch_proto_parse(ch_proto_pool_t *ppool,ch_mmap_file_entry_t *fentry){

	ch_app_proto_t *aproto = _app_proto_get(ppool,fentry);
	
	if(aproto){
	
		ppool->trans_proto->parse(ppool->trans_proto,aproto,fentry);
	}
}


