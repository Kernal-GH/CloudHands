/*
 *
 *      Filename: ch_mail_parser.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-18 14:54:01
 * Last Modified: 2018-05-18 18:10:31
 */

#include "ch_mail_parser.h"
#include "ch_log.h"
#include "ch_shm_format.h"
#include "ch_packet_record.h"
#include "ch_mail_session_entry.h"
#include "ch_mail_session.h"
#include "ch_session_store.h"
#include "ch_mail_common.h"
#include "smtp_parser.h"
#include "imap_parser.h"
#include "pop3_parser.h"

#include "do_mail_parse.c"
#include "do_mail_format.c"
#include "do_mail_create.c"

static void _mail_parser_register(ch_pp_pool_t *pp_pool,ch_mail_parse_context_t *parse_context,uint32_t proto_id){

	ch_pp_parser_t *mail_parser = (ch_pp_parser_t*)ch_pcalloc(pp_pool->mp,sizeof(*mail_parser));

	mail_parser->pp_pool = pp_pool;
	mail_parser->parse_context = parse_context;
	mail_parser->proto_id = proto_id;
	mail_parser->pkt_rcd_type = PKT_RECORD_TYPE_TCP_MAIL;
	mail_parser->session_entry_create = do_mail_session_entry_create;
	mail_parser->request_parse = do_mail_request_parse;
	mail_parser->response_parse = do_mail_response_parse;
	mail_parser->session_format = do_mail_session_format;
	mail_parser->session_entry_clean = do_mail_session_entry_clean;

	ch_pp_parser_register(pp_pool,mail_parser);
}

int ch_mail_parser_init(ch_pp_pool_t *pp_pool,const char *cfname){

	ch_mail_parse_context_t *parse_context = (ch_mail_parse_context_t*)ch_palloc(pp_pool->mp,sizeof(*parse_context));

	parse_context->mcontext = ch_mail_context_create(pp_pool->pwork->pcontext,cfname);

	if(parse_context->mcontext == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot load mail config file:%s",cfname);
		return -1;
	}

	parse_context->content_fpath = ch_fpath_create(pp_pool->mp,parse_context->mcontext->content_dir,
		parse_context->mcontext->create_body_dir_type,0,0);
	
	parse_context->attach_fpath = ch_fpath_create(pp_pool->mp,parse_context->mcontext->attach_dir,
		parse_context->mcontext->create_body_dir_type,0,0);

	if(parse_context->content_fpath == NULL || parse_context->attach_fpath == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create file path for store mail body!");
		return -1;
	}

	_mail_parser_register(pp_pool,parse_context,PROTOCOL_POP3);
	_mail_parser_register(pp_pool,parse_context,PROTOCOL_IMAP);
	_mail_parser_register(pp_pool,parse_context,PROTOCOL_SMTP);

	return 0;
}



