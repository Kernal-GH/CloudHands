/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年6月28日 15时21分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CH_ASSEMBLE_POOL_H
#define CH_ASSEMBLE_POOL_H


typedef struct ch_assemble_pool_t ch_assemble_pool_t;

#include <apr_pools.h>
#include <rte_spinlock.h>
#include <apr_tables.h>
#include "ch_context.h"
#include "ch_assemble_packet.h"
#include "ch_task.h"
#include "ch_assemble_task.h"

/*assemble pools id*/
#define ASSP_TCP 0
#define ASSP_UDP 1

struct ch_assemble_pool_t {

    rte_spinlock_t spin_lock;
    
	ch_context_t *context;
    apr_array_header_t *assemble_tasks;

    int asp_id;

    size_t assemble_tasks_n;

    const char *mmap_file_dir;

    int (*packet_process)(ch_assemble_pool_t *asp,ch_assemble_packet_t *as_pkt);

};

#define ch_assemble_pool_lock(asp) rte_spinlock_lock(&(asp)->spin_lock)
#define ch_assemble_pool_unlock(asp) rte_spinlock_unlock(&(asp)->spin_lock)

static inline void ch_assemble_pool_task_put(ch_assemble_pool_t *asp,ch_task_t *tk){

    apr_array_header_t *arr = asp->assemble_tasks;

    *(ch_task_t**)apr_array_push(arr) = tk; 
}

extern void ch_assemble_pool_init(ch_context_t *context,ch_assemble_pool_t *asp,int asp_id,size_t assemble_tasks_n,
        const char *fdir,
        int (*packet_process)(ch_assemble_pool_t *asp,ch_assemble_packet_t *as_pkt));

extern int ch_assemble_pool_packet_process(ch_assemble_pool_t *asp,ch_assemble_packet_t *as_pkt);

extern ch_assemble_task_t * ch_assemble_pool_task_schedule(ch_assemble_pool_t *asp);

#endif /*CH_ASSEMBLE_POOL_H*/
