/*
 * =====================================================================================
 *
 *       Filename:  ch_core.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月16日 10时36分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_CORE_H
#define CH_CORE_H

typedef struct ch_core_t ch_core_t;

#include <apr_pools.h>
#include "ch_task.h"

struct ch_core_t {
    ch_task_t *task;
    
    unsigned int core_id;
    unsigned int socket;

    unsigned assigned:1;
};

extern ch_core_t * ch_core_create(apr_pool_t *mp,unsigned int core_id);

extern void ch_core_task_assign(ch_core_t *core,ch_task_t *task);

extern void ch_core_task_unassign(ch_core_t *core);

#endif /*CH_CORE_H*/

