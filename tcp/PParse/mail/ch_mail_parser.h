/*
 *
 *      Filename: ch_mail_parser.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-05-18 13:55:39
 * Last Modified: 2018-05-18 13:55:39
 */

#ifndef CH_MAIL_PARSER_H
#define CH_MAIL_PARSER_H

typedef struct ch_mail_parse_context_t ch_mail_parse_context_t;

#include "ch_pp_pool.h"
#include "ch_mail_context.h"

struct ch_mail_parse_context_t {

	ch_mail_context_t *mcontext;

	ch_fpath_t *content_fpath;

	ch_fpath_t *attach_fpath;

};

extern int ch_mail_parser_init(ch_pp_pool_t *pp_pool,const char *cfname);


#endif /*CH_MAIL_PARSER_H*/
