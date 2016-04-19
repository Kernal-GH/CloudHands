/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_session_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月14日 11时23分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef Ch_ASSEMBLE_SESSION_POOL_H
#define Ch_ASSEMBLE_SESSION_POOL_H

typedef struct ch_assemble_session_pool_t ch_assemble_session_pool_t;
#include <apr_pools.h>
#include "ch_hash_pool.h"
#include "ch_assemble_session.h"
#include "ch_assemble_task.h"

struct ch_assemble_session_pool_t {

    apr_pool_t *mp;
    ch_assemble_task_t *astask;

    ch_hash_pool_t *hash_pool;

    size_t n_sessions;

};


extern ch_assemble_session_pool_t * ch_assemble_session_pool_create(ch_assemble_task_t *astask,
        apr_pool_t *mp,size_t n_sessions_limit);


extern void ch_assemble_session_pool_destroy(ch_assemble_session_pool_t *ass_pool);

extern ch_assemble_session_t * ch_assemble_session_pool_entry_create(ch_assemble_session_pool_t *ass_pool,ch_session_request_t *sreq,
        ch_four_tuple_t *tuple);


extern ch_assemble_session_t * ch_assemble_session_pool_entry_find(ch_assemble_session_pool_t *ass_pool,ch_four_tuple_t *tuple);


extern void ch_assemble_session_pool_entry_free(ch_assemble_session_pool_t *ass_pool,ch_assemble_session_t *ass);

#endif /*Ch_ASSEMBLE_SESSION_POOL_H*/

