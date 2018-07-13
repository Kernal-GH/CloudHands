/*
 *
 *      Filename: ch_smon_parser.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-13 13:45:22
 * Last Modified: 2018-07-13 13:45:22
 */

#ifndef CH_SMON_PARSER_H
#define CH_SMON_PARSER_H

typedef struct ch_smon_parse_context_t ch_smon_parse_context_t;

#include "ch_pp_pool.h"
#include "ch_fpath.h"
#include "ch_smon_context.h"

struct ch_smon_parse_context_t {

	ch_smon_context_t *scontext;

	ch_fpath_t *req_fpath;
	ch_fpath_t *res_fpath;

};

extern int ch_smon_parser_init(ch_pp_pool_t *pp_pool,const char *cfname);

#endif /*CH_SMON_PARSER_H*/
