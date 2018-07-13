/*
 *
 *      Filename: ch_smon_parser.c
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-13 14:23:08
 * Last Modified: 2018-07-13 14:29:24
 */


#include "ch_smon_parser.h"
#include "ch_log.h"
#include "ch_shm_format.h"
#include "ch_packet_record.h"
#include "ch_smon_session_entry.h"
#include "ch_session_store.h"

#include "do_smon_parse.c"
#include "do_smon_format.c"
#include "do_smon_create.c"

static ch_pp_parser_t smon_pp_parser = {

	.pp_pool = NULL,
	.parse_context = NULL,
	.proto_id = PROTOCOL_SMON,
	.pkt_rcd_type = PKT_RECORD_TYPE_TCP_SMON,
	.session_entry_create = do_smon_session_entry_create,
	.request_parse = do_smon_request_parse,
	.response_parse = do_smon_response_parse,
	.session_format = do_smon_session_format,
	.session_entry_clean = do_smon_session_entry_clean
};

int ch_smon_parser_init(ch_pp_pool_t *pp_pool,const char *cfname){

	ch_smon_parse_context_t *parse_context = (ch_smon_parse_context_t*)ch_palloc(pp_pool->mp,sizeof(*parse_context));

	parse_context->scontext = ch_smon_context_create(pp_pool->pwork->pcontext,cfname);
	if(parse_context->scontext == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot load smon config file:%s",cfname);
		return -1;
	}

	parse_context->req_fpath = ch_fpath_create(pp_pool->mp,parse_context->scontext->req_body_dir,
		parse_context->scontext->create_body_dir_type,0,0);
	
	parse_context->res_fpath = ch_fpath_create(pp_pool->mp,parse_context->scontext->res_body_dir,
		parse_context->scontext->create_body_dir_type,0,0);

	if(parse_context->req_fpath == NULL || parse_context->res_fpath == NULL){
	
		ch_log(CH_LOG_ERR,"Cannot create file path for store smon body!");
		return -1;
	}

	smon_pp_parser.parse_context = parse_context;

	ch_pp_parser_register(pp_pool,&smon_pp_parser);

	return 0;
}

