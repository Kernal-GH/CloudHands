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

#include "ch_assemble_task.h"
#include "ch_assemble_session.h"

extern void ch_assemble_do(ch_assemble_task_t *astask,struct rte_mbuf *mbuf);

#endif /*CH_ASSEMBLE_H*/
