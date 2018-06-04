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

#include "ch_context.h"
#include "ch_mmap_file_format.h"
#include "ch_assemble_task.h"

struct ch_assemble_t {

    ch_mmap_file_format_t *mmfmt;
    int assemble_id;

    ch_assemble_task_t *as_task;

    uint64_t cur_id;
    void (*do_assemble)(ch_assemble_t *as,struct rte_mbuf *mbuf);
};

static inline uint64_t ch_assemble_id_get(ch_assemble_t *as){

    uint64_t id = as->cur_id;
    as->cur_id = (as->cur_id+1)%ULONG_MAX;
    return id;
} 

extern int ch_assemble_init(ch_context_t *context,ch_assemble_t *as, int assemble_id,
        const char *fdir,
		const char *app_name,
		uint64_t fsize,uint64_t fentry_size,
        void (*do_assemble)(ch_assemble_t *as,struct rte_mbuf *m));

extern void ch_assemble_fin(ch_assemble_t *as);

extern void ch_assemble_do(ch_assemble_t *as,struct rte_mbuf *mbuf);

#endif /*CH_ASSEMBLE_H*/
