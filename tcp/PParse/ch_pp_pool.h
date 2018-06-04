/*
 *
 *      Filename: ch_pp_pool.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-14 15:55:43
 * Last Modified: 2018-05-14 15:55:43
 */

#ifndef CH_PP_POOL_H
#define CH_PP_POOL_H

typedef struct ch_pp_pool_t ch_pp_pool_t;
typedef struct ch_pp_parser_t ch_pp_parser_t;
typedef struct ch_session_format_result_t ch_session_format_result_t;

#include "ch_pp_work.h"
#include "ch_tables.h"
#include "ch_tcp_record.h"
#include "ch_session_entry.h"
#include "ch_session_entry_pool.h"
#include "ch_session_store.h"
#include "ch_ptable.h"

/* Parse data status returned! */
#define PARSE_DONE 0
#define PARSE_CONTINUE 1
#define PARSE_BREAK 2

struct ch_pp_pool_t {

	ch_pool_t *mp;

	ch_pp_work_t *pwork;

	ch_array_header_t *pp_parsers;

	ch_session_store_t *sstore;

	ch_session_entry_pool_t *sentry_pool;
};

struct ch_pp_parser_t {

	ch_pp_pool_t *pp_pool;

	void *parse_context;

	uint32_t proto_id;
	uint8_t  pkt_rcd_type;

	void *(*session_entry_create)(ch_session_entry_t *sentry);
	
	int (*request_parse)(ch_session_entry_t *sentry,void *data,size_t dlen);
	int (*response_parse)(ch_session_entry_t *sentry,void *data,size_t dlen);

	void (*session_format)(msgpack_packer *pk,void *session);

	void (*session_entry_clean)(ch_session_entry_t *sentry);

};

static inline void ch_pp_parser_register(ch_pp_pool_t *pp_pool,ch_pp_parser_t *pp_parser){

	pp_parser->pp_pool = pp_pool;

	*(ch_pp_parser_t**)ch_array_push(pp_pool->pp_parsers) = pp_parser;

}

static inline ch_pp_parser_t *ch_pp_parser_find(ch_pp_pool_t *pp_pool,ch_tcp_record_t *tcp_record){

	int i;
	ch_pp_parser_t **pps,*pp = NULL;

	pps = (ch_pp_parser_t**)pp_pool->pp_parsers->elts;
	
	for(i = 0;i<pp_pool->pp_parsers->nelts;i++){
	
		pp = pps[i];

		if(tcp_record->protocol_id == pp->proto_id)
			return pp;
	}


	return NULL;
}

extern ch_pp_pool_t * ch_pp_pool_create(ch_pp_work_t *pwork);

extern void ch_pp_session_parse(ch_pp_pool_t *pp_pool,ch_tcp_record_t *tcp_record);

#endif /*CH_PP_POOL_H*/
