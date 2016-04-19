/*
 * =====================================================================================
 *
 *       Filename:  ch_assemble_task.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年02月16日 18时01分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  shajf (), csp001314@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CH_ASSEMBLE_TASK_H
#define CH_ASSEMBLE_TASK_H

typedef struct ch_assemble_task_t ch_assemble_task_t;

#include <apr_pools.h>
#include <rte_ring.h>
#include "ch_task.h"
#include "ch_assemble_session.h"
#include "ch_packet.h"
#include "ch_assemble_session_pool.h"
#include "ch_util.h"
#include "ch_session_request_pool.h"

struct ch_assemble_task_t {

    ch_task_t task;

    ch_context_t *context;

    apr_pool_t *mp;
    ch_assemble_session_pool_t *ass_pool;

    struct rte_ring *pkts;

    uint16_t task_id;
    size_t ring_size;

    void *assemble_buf;
    size_t as_buf_size;
};

#define ch_assemble_task_n_sessions_get(astask) ((astask)->ass_pool->n_sessions)


/*Create Assemble task
 * @context,the instance of context
 * @task_id,the task id of this assemble task
 * return: if ok,return the instance of the assemble task,otherwise return NULL*/
extern ch_task_t * ch_assemble_task_create(ch_context_t *context,uint16_t task_id);


/*Find a assemble session from this assemble task by tcp four tuple
 * @astask,the instance of assemble task
 * @pinfo,the instance of packet infomations
 * return: if ok,return the instance of assemble session found,otherwise return NULL*/
extern ch_assemble_session_t * ch_assemble_task_session_find(ch_assemble_task_t *astask,ch_packet_info_t *pinfo);


/*Create a assemble session 
 * */
extern ch_assemble_session_t *ch_assemble_task_session_create(ch_assemble_task_t *astask,ch_session_request_t *sreq,ch_packet_info_t *pinfo);

/*Put a low level packet into assemble task's ring waiting to be assembled by assemble task
 * @astask,the instance of assemble task
 * @pinfo,the instance of packet information that contains ips,ports and packet payload 
 * return: if ok,return the 0,otherwise return -1*/
extern int ch_assemble_task_pkt_put(ch_assemble_task_t *astask,ch_packet_info_t *pinfo);

#endif /*CH_ASSEMBLE_TASK_H*/

