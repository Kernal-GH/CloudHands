/*
 *
 *      Filename: ch_process_interface_sa_context.h
 *
 *        Author: shajf,csp001314@gmail.com
 *   Description: ---
 *        Create: 2018-03-30 12:25:51
 * Last Modified: 2018-03-30 12:25:51
 */

#ifndef CH_PROCESS_INTERFACE_SA_CONTEXT_H
#define CH_PROCESS_INTERFACE_SA_CONTEXT_H

typedef struct ch_process_interface_sa_context_t ch_process_interface_sa_context_t;
#include "ch_mpool.h"
#include "ch_process_interface.h"


struct ch_process_interface_sa_context_t {

	ch_pool_t *mp;

	ch_process_interface_t *pint;

	int use_rss;

	const char *pool_name;

	const char *qprefix;

	uint32_t qnumber;

	uint32_t qsize;

};


extern ch_process_interface_sa_context_t *ch_process_interface_sa_context_create(ch_pool_t *mp,const char *cfname,int is_write);

#endif /*CH_PROCESS_INTERFACE_SA_CONTEXT_H*/
