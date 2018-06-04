/*
 *
 *      Filename: ch_proto_work.h
 *
 *        Author: shajf,csp001314@163.com
 *   Description: ---
 *        Create: 2016-10-27 00:32:12
 * Last Modified: 2016-10-27 00:32:12
 */

#ifndef CH_PROTO_WORK_H
#define CH_PROTO_WORK_H

#include "ch_proto_context.h"

extern int ch_proto_work_run(ch_proto_context_t *pcontext);

extern void ch_proto_work_exit(ch_proto_context_t *pcontext);

#endif /* CH_PROTO_WORK_H */
