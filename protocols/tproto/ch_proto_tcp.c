/*
 *
 *      Filename: ch_proto_tcp.c
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 23:56:01
 * Last Modified: 2017-01-10 11:26:40
 */

#include "ch_log.h"
#include "ch_proto_tcp.h"
#include "ch_http_proto.h"
#include "ch_debug_proto.h"
#include "ch_mail_proto.h"
#include "ch_telnet_proto.h"
#include "ch_ftp_proto.h"

static inline int _is_flush_or_close(unsigned int packet_type){

	return packet_type == PACKET_TYPE_FLUSH||packet_type == PACKET_TYPE_CLOSE;
}

static void _flush_or_close_packet_process(ch_app_proto_t *aproto,ch_session_entry_t *sentry){

	ch_table_entry_free(aproto->htbl,(ch_table_entry_t*)sentry);

}

static void _status_parsed_process(ch_app_proto_t *aproto,ch_session_entry_t *sentry,int rc){

	switch(rc){
	
	case PARSE_DONE:
	case PARSE_BREAK:
		_flush_or_close_packet_process(aproto,sentry);
		break;
	
	default:
		break;
	}

} 

static void _data_packet_process(ch_app_proto_t *aproto,
	ch_session_entry_t *sentry,ch_mmap_file_entry_t *fentry){

	int rc;

	if(fentry->data&&fentry->data_len>0){
	
		if(fentry->session_direct == SESSION_DIRECT_REQ){

			rc = aproto->parse_request(aproto,sentry,fentry->data,fentry->data_len,NULL);

		}else{
			rc = aproto->parse_response(aproto,sentry,fentry->data,fentry->data_len,NULL);
		
		}

		/* handle the status returned */
		_status_parsed_process(aproto,sentry,rc);
	}
}

/* callback function proto parse */
static void _tcp_parse(ch_proto_trans_t *trans_proto,ch_app_proto_t *aproto,ch_mmap_file_entry_t *fe){

	/* unused */
	trans_proto = trans_proto;

	size_t c = 0;

	ch_session_entry_t *sentry = NULL;

	/* find the session */
	sentry = (ch_session_entry_t*)ch_table_entry_find_create(aproto->htbl,(void*)fe);
	if(sentry == NULL){
	
		ch_log(CH_LOG_ERR,"find or create session entry failed!");
		return;
	}

	/* handle flush &close entry*/
	if(_is_flush_or_close(fe->packet_type)){
	
		_flush_or_close_packet_process(aproto,sentry);

	}else{
		/* data */
		_data_packet_process(aproto,sentry,fe);
	}

	c = ch_table_entries_timeout_free(aproto->htbl,NULL);

	if(c>0){
	
		ch_log(CH_LOG_WARN,"Free timeout session entries:%lu",(unsigned long)c);
	}

}

int ch_proto_tcp_init(ch_proto_pool_t *ppool){

	ch_proto_context_t *pcontext = ppool->pcontext;
	ch_app_proto_t *aproto;

	int i;

	ch_proto_tcp_t *tproto = (ch_proto_tcp_t*)apr_palloc(ppool->pcontext->mp,sizeof(*tproto));

	tproto->trans_proto.parse = _tcp_parse;
	tproto->trans_proto.trans_proto_type = TPT_TCP;

	/* init to NULL */
	for(i = 0; i<PROTOCOL_MAX;i++){
	
		tproto->trans_proto.app_protos[i] = NULL;
	}

	/* register tcp protocol */
	ppool->trans_proto = (ch_proto_trans_t*)tproto;
	
	/* register app protocols based on tcp */

	if(pcontext->http_proto_cfile){
	
		/* create and register http proto */
		aproto = ch_http_proto_create(pcontext->mp,pcontext);
		if(aproto){
		
			ch_app_proto_register(ppool,aproto);
		}else{
		
			ch_log(CH_LOG_ERR,"Cannot create and register http protocol!");
			return -1;
		}
	}

	if(pcontext->mail_proto_cfile){
	
		/* register mail protos(smtp,imap,pop3) */
		if(ch_mail_proto_register(ppool)<=0){
		
			ch_log(CH_LOG_ERR,"Cannot create and register mail protocol!");
			return -1;

		}
	}

	if(pcontext->debug_proto_cfile){
	
		/* create and register debug proto */
		aproto = ch_debug_proto_create(pcontext->mp,pcontext);
		if(aproto){
		
			ch_app_proto_register(ppool,aproto);
		}else{
		
			ch_log(CH_LOG_ERR,"Cannot create and register debug protocol!");
			return -1;
		}
	}

	if (pcontext->telnet_proto_cfile) {
		/* create and register telnet proto */
		aproto = ch_telnet_proto_create(pcontext->mp, pcontext);
		if (aproto) {
			ch_app_proto_register(ppool, aproto);
		} else {
			ch_log(CH_LOG_ERR, "Cannot create and register telnet protocol!");
			return -1;
		}
	}

	if (pcontext->ftp_proto_cfile) {
		/* create and register ftp proto */
		aproto = ch_ftp_proto_create(pcontext->mp, pcontext);

		if (aproto) {
			ch_app_proto_register(ppool, aproto);
		} else {
			ch_log(CH_LOG_ERR, "Cannot create and register ftp protocol!");
			return -1;
		}
	}
	
	/* ok! */
	return 0;
}
