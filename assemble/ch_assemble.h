/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年04月18日 14时41分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ASSEMBLE_H
#define CH_ASSEMBLE_H

typedef struct ch_assemble_t ch_assemble_t;

#include "ch_assemble_session.h"
#include "ch_assemble_session_pool.h"
#include "ch_mmap_file_format.h"

struct ch_assemble_t {

    ch_context_t *context;
    ch_assemble_session_pool_t *ass_pool;
    ch_mmap_file_format_t *mmfmt;

};

extern ch_assemble_t * ch_assemble_create(ch_context_t *context,unsigned int task_id);

extern void ch_assemble_destroy(ch_assemble_t *as);

extern void ch_assemble_do(ch_assemble_t *as,struct rte_mbuf *mbuf);

#endif /*CH_ASSEMBLE_H*/
