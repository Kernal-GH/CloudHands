/*
 *
 *      Filename: ch_debug_proto.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-12-05 22:11:54
 * Last Modified: 2016-12-05 22:11:54
 */

#ifndef CH_DEBUG_PROTO_H
#define CH_DEBUG_PROTO_H

typedef struct ch_debug_proto_t ch_debug_proto_t;

#include "ch_debug_context.h"
#include "ch_proto_context.h"
#include "ch_proto_pool.h"
#include "ch_fpath.h"

struct ch_debug_proto_t {

	ch_app_proto_t aproto;

	ch_debug_context_t *dcontext;

	ch_fpath_t *req_fpath;
	ch_fpath_t *res_fpath;
};


extern ch_app_proto_t * ch_debug_proto_create(apr_pool_t *mp,ch_proto_context_t *pcontext);


#endif /* CH_DEBUG_PROTO_H */
