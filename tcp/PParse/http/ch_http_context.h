/*
 * =====================================================================================
 *
 *       Filename:  ch_http_context.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年7月12日 12时41分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_HTTP_CONTEXT_H
#define CH_HTTP_CONTEXT_H

typedef struct ch_http_context_t ch_http_context_t;

#include "ch_pp_context.h"

struct ch_http_context_t {

    ch_pp_context_t *pcontext;

	const char *req_body_dir;
	const char *res_body_dir;


	int create_body_dir_type;

};

extern ch_http_context_t *ch_http_context_create(ch_pp_context_t *pcontext,const char *cfname);

#endif /*CH_HTTP_CONTEXT_H*/
