/*
 *
 *      Filename: ch_smon_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-07-13 13:48:41
 * Last Modified: 2018-07-13 13:48:41
 */

#ifndef CH_SMON_CONTEXT_H
#define CH_SMON_CONTEXT_H

typedef struct ch_smon_context_t ch_smon_context_t;

#include "ch_pp_context.h"

struct ch_smon_context_t {

    ch_pp_context_t *pcontext;

	const char *req_body_dir;
	const char *res_body_dir;


	int create_body_dir_type;

};

extern ch_smon_context_t *ch_smon_context_create(ch_pp_context_t *pcontext,const char *cfname);


#endif /*CH_SMON_CONTEXT_H*/
