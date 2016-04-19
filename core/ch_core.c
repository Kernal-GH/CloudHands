/*
 * =====================================================================================
 *
 *       Filename:  ch_core.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年12月16日 10时47分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf, csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <rte_lcore.h>
#include "ch_core.h"

ch_core_t * ch_core_create(apr_pool_t *mp,unsigned int core_id){

    ch_core_t *core = (ch_core_t*)apr_palloc(mp,sizeof(ch_core_t));

    core->task = NULL;
    core->core_id = core_id;
    core->socket = (unsigned int) rte_lcore_to_socket_id(core_id);

    core->assigned = 0;

    return core;
}

void ch_core_task_assign(ch_core_t *core,ch_task_t *task){

    core->task = task;
    core->assigned = 1;
}

void ch_core_task_unssign(ch_core_t *core){

    core->task = NULL;
    core->assigned = 0;
}
