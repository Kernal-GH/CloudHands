/*
 *
 *      Filename: ch_proto_udp.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-30 19:15:04
 * Last Modified: 2016-11-02 03:27:40
 */
#include "ch_log.h"
#include "ch_proto_udp.h"
#include "ch_dns_proto.h"

/* callback function proto parse */
static void _udp_parse(ch_proto_trans_t *trans_proto,ch_app_proto_t *aproto,ch_mmap_file_entry_t *fe){

	/* unused */
	trans_proto = trans_proto;

	ch_session_entry_t sentry;

	ch_session_entry_init(&sentry,fe);

	if(fe->data&&fe->data_len>0){
	
		if(aproto->is_request(aproto,fe,NULL))
			aproto->parse_request(aproto,&sentry,fe->data,fe->data_len,NULL);
		else
			aproto->parse_response(aproto,&sentry,fe->data,fe->data_len,NULL);
	}
}

int ch_proto_udp_init(ch_proto_pool_t *ppool){

	int i;
	ch_proto_context_t *pcontext = ppool->pcontext;

	ch_app_proto_t *aproto;

	ch_proto_udp_t *uproto = (ch_proto_udp_t*)apr_palloc(pcontext->mp,sizeof(*uproto));

	uproto->trans_proto.parse = _udp_parse;
	uproto->trans_proto.trans_proto_type = TPT_UDP;

	/* init to NULL */
	for(i = 0; i<PROTOCOL_MAX;i++){
	
		uproto->trans_proto.app_protos[i] = NULL;
	}

	/* register tcp protocol */
	ppool->trans_proto = (ch_proto_trans_t*)uproto;

	/* register app protocols based on udp */
	if(pcontext->dns_proto_cfile){
	

		/* create and register dns protocol */
		aproto = ch_dns_proto_create(pcontext);
		if(aproto){
		
			ch_app_proto_register(ppool,aproto);
		}else{
		
			ch_log(CH_LOG_ERR,"Cannot create and register dns protocol!");
			return -1;
		}
	}

	/* ok! */

	return 0;
}
