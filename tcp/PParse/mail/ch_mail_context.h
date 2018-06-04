/*
 *
 *      Filename: ./protocols/mail/ch_mail_context.h
 *
 *        Author: g.goodian@gmail.com
 *   Description: ---
 *        Create: 2016-07-29 17:11:13
 * Last Modified: 2016-07-29 17:11:13
 */

#ifndef __CH_PARSER_MAIL_CONTEXT_H__
#define __CH_PARSER_MAIL_CONTEXT_H__

#include "ch_pp_context.h"

typedef struct ch_mail_context_t ch_mail_context_t;

struct ch_mail_context_t {
    
	ch_pp_context_t *pcontext;

	const char *content_dir; /* store path for mail content */
	const char *attach_dir; /* store path for mail attachment */

	int create_body_dir_type; /* dir type for profile content and attach dir */
};

extern ch_mail_context_t *ch_mail_context_create(ch_pp_context_t *pcontext,const char *cfname);

#endif //__CH_PARSER_MAIL_CONTEXT_H__
